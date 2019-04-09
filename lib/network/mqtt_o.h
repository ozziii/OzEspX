#ifndef MQTT_O_H
#define MQTT_O_H

#include <vector>

#ifdef ESP32
#include <WiFi.h>
#include <FS.h>
#else
#include <ESP8266WiFi.h>
#include <Hash.h>
#endif

#include <configuration.h>
#include <debug_o.h>
#include <setting.h>

#ifdef MQTT_USE_ASYNC
#include <AsyncMqttClient.h>
static AsyncMqttClient _mqtt;
#else
#include <PubSubClient.h>
static PubSubClient _mqtt;
static WiFiClient _mqtt_client;
#endif

static std::vector<const char *> _mqtt_topics;
typedef std::function<void(char *topic, char *payload, unsigned int len)> OnMessageMqttCallback;
static OnMessageMqttCallback messageMqttCallback;

class mqtt_o
{
  public:
    static void mqttBegin();
    static void mqttLoop();
    static bool mqttPublish(const char *topic, const char *message);
    static void mqttSubscribe(const char *topic);
    static void mqttSetCallback(OnMessageMqttCallback callback);
    static void mqttDisconnect();

  private:
    static void _mqttConnect();
    static void _mqttOnDisconnect();
    static void _mqttOnConnect();
    static void _mqttSubscribe(const char *topic);
    static void _mqttOnMessage(char *topic, char *payload, unsigned int len);
    static String _availability_topic();
};

#endif