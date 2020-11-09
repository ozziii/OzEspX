#include <mqtt_o.h>

static unsigned long _mqtt_reconnect_delay = MQTT_RECONNECT_DELAY_MIN;
static unsigned long _mqtt_last_reconnect = 0;
static bool _mqtt_is_connected = false;

void mqtt_o::mqttBegin()
{
#ifdef MQTT_USE_ASYNC

    _mqtt.onConnect([](bool sessionPresent) {
        _mqttOnConnect();
    });
    _mqtt.onDisconnect([](AsyncMqttClientDisconnectReason reason) {
#ifdef DEBUG_ERROR
        if (reason == AsyncMqttClientDisconnectReason::TCP_DISCONNECTED)
        {
            OZ_LOG_E_P(PSTR("[MQTT] TCP Disconnected\n"));
        }
        if (reason == AsyncMqttClientDisconnectReason::MQTT_IDENTIFIER_REJECTED)
        {
            OZ_LOG_E_P(PSTR("[MQTT] Identifier Rejected\n"));
        }
        if (reason == AsyncMqttClientDisconnectReason::MQTT_SERVER_UNAVAILABLE)
        {
            OZ_LOG_E_P(PSTR("[MQTT] Server unavailable\n"));
        }
        if (reason == AsyncMqttClientDisconnectReason::MQTT_MALFORMED_CREDENTIALS)
        {
            OZ_LOG_E_P(PSTR("[MQTT] Malformed credentials\n"));
        }
        if (reason == AsyncMqttClientDisconnectReason::MQTT_NOT_AUTHORIZED)
        {
            OZ_LOG_E_P(PSTR("[MQTT] Not authorized\n"));
        }
#endif
        _mqttOnDisconnect();
    });
    _mqtt.onMessage([](char *topic, char *payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
        _mqttOnMessage(topic, payload, len);
    });
    _mqtt.onSubscribe([](uint16_t packetId, uint8_t qos) {
        OZ_LOG_I_P(PSTR("[MQTT] Subscribe ACK for PID %d\n"), packetId);
    });
    _mqtt.onPublish([](uint16_t packetId) {
        OZ_LOG_I_P(PSTR("[MQTT] Publish ACK for PID %d\n"), packetId);
    });

#else // not MQTT_USE_ASYNC
    _mqtt.setCallback([](char *topic, byte *payload, unsigned int length) {
        _mqttOnMessage(topic, (char *)payload, length);
    });

#endif // end MQTT_USE_ASYNC
}

void mqtt_o::mqttLoop()
{
#ifdef MQTT_USE_ASYNC

    _mqttConnect();

#else // not MQTT_USE_ASYNC

    if (_mqtt.connected())
    {
        _mqtt.loop();
    }
    else
    {
        if (_mqtt_is_connected)
        {
            _mqttOnDisconnect();
            _mqtt_is_connected = false;
        }
        _mqttConnect();
    }

#endif // End MQTT_USE_ASYNC
}

bool mqtt_o::mqttPublish(const char *topic, const char *message)
{
    if (_mqtt.connected())
    {
#ifdef MQTT_USE_ASYNC
        unsigned int packetId = _mqtt.publish(topic, MQTT_QoS, MQTT_RETAIN, message);
        OZ_LOG_I_P(PSTR("[MQTT] Sending %s => %s (PID %d)\n"), topic, message, packetId);

#else
        _mqtt.publish(topic, message, MQTT_RETAIN);
        OZ_LOG_I_P(PSTR("[MQTT] Sending %s => %s\n"), topic, message);

#endif
        return true;
    }
    return false;
}

void mqtt_o::mqttSubscribe(const char *topic)
{
    _mqtt.state();
    _mqtt_topics.push_back(topic);
}

void mqtt_o::mqttSetCallback(OnMessageMqttCallback callback)
{
    messageMqttCallback = callback;
}

void mqtt_o::mqttDisconnect()
{
    _mqttOnDisconnect();
    _mqtt.disconnect();
}

//****************  PRIVATE ***************************************************************

void mqtt_o::_mqttConnect()
{
    // Do not connect if already connected
    if (_mqtt.connected())
        return;

    // Check reconnect interval

    if (millis() - _mqtt_last_reconnect < _mqtt_reconnect_delay)
        return;
    _mqtt_last_reconnect = millis();

    // Increase the reconnect delay
    if (_mqtt_reconnect_delay <= MQTT_RECONNECT_DELAY_MAX)
    {
        _mqtt_reconnect_delay += MQTT_RECONNECT_DELAY_STEP;
    }

    String host = SETTING_P(PSTR("MQTTSIP"));

    if (host.length() == 0)
    {
        OZ_LOG_W_P(PSTR("[MQTT] No Ip Set \n"));
        return;
    }

    uint16_t port = SETTING_P_I(PSTR("MQTTPORT"));
    String _mqtt_user = SETTING_P(PSTR("MQTTUSER"));
    String _mqtt_pass = SETTING_P(PSTR("MQTTPASS"));
    String _mqtt_clientid = SETTING_P(PSTR("espname"));

#ifdef MQTT_USE_ASYNC

    _mqtt.setServer(host.c_str(), port);
    _mqtt.setClientId(_mqtt_clientid.c_str());
    _mqtt.setKeepAlive(MQTT_KEEPALIVE);
    _mqtt.setCleanSession(false);
    //_mqtt.setWill(MQTT_WILL, MQTT_QoS, MQTT_RETAIN, "0");
    if ((strlen(_mqtt_user.c_str()) > 0) && (strlen(_mqtt_pass.c_str()) > 0))
    {
        OZ_LOG_I_P(PSTR("[MQTT] Connecting as user %s\n"), _mqtt_user.c_str());

        _mqtt.setCredentials(_mqtt_user.c_str(), _mqtt_pass.c_str());
    }

    _mqtt.connect();

#else // not MQTT_USE_ASYNC

    bool response = true;

    _mqtt.setClient(_mqtt_client);

    _mqtt.setServer(host.c_str(), port);

    OZ_LOG_I_P(PSTR("[MQTT] TRY TO CONNECT Ip %s port %d  User %s Passw %s\n"), host.c_str(), port, _mqtt_user.c_str(), _mqtt_pass.c_str());

    if ((strlen(_mqtt_user.c_str()) > 0) && (strlen(_mqtt_pass.c_str()) > 0))
    {
        OZ_LOG_I_P(PSTR("[MQTT] Connecting as user %s\n"), _mqtt_user.c_str());

        //               connect(const char *id, const char *user, const char *pass, const char* willTopic, uint8_t willQos, boolean willRetain, const char* willMessage)
        response = _mqtt.connect(_mqtt_clientid.c_str(), _mqtt_user.c_str(), _mqtt_pass.c_str(), _availability_topic().c_str(), MQTT_QoS, MQTT_RETAIN, MQTT_WILL);
    }
    else
    {
        //boolean PubSubClient::connect(const char *id, const char* willTopic, uint8_t willQos, boolean willRetain, const char* willMessage)
        response = _mqtt.connect(_mqtt_clientid.c_str(), _availability_topic().c_str(), MQTT_QoS, MQTT_RETAIN, MQTT_WILL);
    }

    if (response)
    {
        _mqttOnConnect();
    }
    else
    {
        OZ_LOG_W_P(PSTR("[MQTT] Connection failed\n"));
    }

#endif // end MQTT_USE_ASYNC
}

void mqtt_o::_mqttOnDisconnect()
{
    OZ_LOG_I_P(PSTR("[MQTT] Disconnected!\n"));

    //  Try To solve problem
    //  [E][WiFiClient.cpp:74] connect(): lwip_connect_r: 113
    //
    //  Code is fine, error 113 is ECONNABORTED (Connection aborted).
    //  Try flushing the client or reading all available data.
    //  client.readStringUntil('\r'); does not read it all

    //_mqtt_client.readStringUntil('\r');
    _mqtt_client.flush();

    // END try
}

void mqtt_o::_mqttOnConnect()
{
    OZ_LOG_I_P(PSTR("[MQTT] Connected!\n"));

    _mqtt_is_connected = true;
    _mqtt_reconnect_delay = MQTT_RECONNECT_DELAY_MIN;

    for (int i = 0; i < _mqtt_topics.size(); i++)
    {
        _mqttSubscribe(_mqtt_topics[i]);
    }

    mqttPublish(_availability_topic().c_str(), MQTT_PAYLOAD_AVAILABLE);
}

void mqtt_o::_mqttSubscribe(const char *topic)
{
    if (_mqtt.connected() && (strlen(topic) > 0))
    {
#ifdef MQTT_USE_ASYNC
        unsigned int packetId = _mqtt.subscribe(topic, MQTT_QoS);
        OZ_LOG_I_P(PSTR("[MQTT] Subscribing to %s (PID %d)\n"), topic, packetId);

#else
        _mqtt.subscribe(topic, MQTT_QoS);
        OZ_LOG_I_P(PSTR("[MQTT] Subscribing to %s\n"), topic);

#endif
    }
}

void mqtt_o::_mqttOnMessage(char *topic, char *payload, unsigned int len)
{
    messageMqttCallback(topic, payload, len);
}

String mqtt_o::_availability_topic()
{
    String stret;
    stret += SETTING_P(PSTR("espname"));
    stret += char(MQTT_TOPIC_SEPARATOR_CHAR);
    stret += MQTT_AVAILABLE_TOPIC;
    return stret;
}