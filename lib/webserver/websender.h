#ifndef WEB_SENDER_H
#define WEB_SENDER_H

#include <ESPAsyncWebServer.h>
#include <configuration.h>
#include <setting.h>
#include <AsyncJson.h>
#include <ArduinoJson.h>

typedef std::function<void(char *topic, char *payload, unsigned int len)> OnWebMqttCallback;

class WebSender
{
  public:
    void home(AsyncWebServerRequest *request);
    void homeJson(AsyncWebServerRequest *request);
    void config(AsyncWebServerRequest *request);
    void save_config(AsyncWebServerRequest *request);
    void unlockSystem(AsyncWebServerRequest *request);
    void execute_mqtt(AsyncWebServerRequest *request,OnWebMqttCallback callback);
};

#endif