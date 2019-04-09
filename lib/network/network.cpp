#include "network.h"

network::network() {}

void network::begin(OnMessageMqttCallback callback)
{
    wifi.Begin();
    mqtt_o::mqttBegin();
    mqtt_o::mqttSetCallback(callback);
}

void network::loop()
{
    if (wifi.loop())
    {
        mqtt_o::mqttLoop();
    }
 
}

void network::subscribe(const char *topic)
{
    mqtt_o::mqttSubscribe(topic);
}

bool network::send(const char *topic, const char *message)
{
    return mqtt_o::mqttPublish(topic, message);
}

bool network::wifiIsConnect()
{
    return wifi.connected();
}

String network::getIpAddress()
{
    //return wifi.GetAddress();
    return "";
}

network Network;