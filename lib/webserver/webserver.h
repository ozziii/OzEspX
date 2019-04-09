
#ifndef WEB_H
#define WEB_H

#include <ESPAsyncWebServer.h>
#include <AsyncJson.h>
#include <ArduinoJson.h>

#include <configuration.h>
#include <debug_o.h>
#include <setting.h>
#include <network.h>
#include <websender.h>

static AsyncWebServer *_server;

static OnWebMqttCallback _Web_callback;

static bool webAuthenticate(AsyncWebServerRequest *request)
{
    String UserName = SETTING("WEBUSER");
    String Password = SETTING("WEBPASS");
    return request->authenticate(UserName.c_str(), Password.c_str());
}

static void _onHome(AsyncWebServerRequest *request)
{
#ifdef WEB_SECURE
    if (!webAuthenticate(request))
    {
        return request->requestAuthentication(SETTING("espname").c_str());
    }
#endif

    WebSender sender;
    sender.home(request);
}

static void _onHomeJson(AsyncWebServerRequest *request)
{
#ifdef WEB_SECURE
    if (!webAuthenticate(request))
    {
        return request->requestAuthentication(SETTING("espname").c_str());
    }
#endif

    WebSender sender;
    sender.homeJson(request);
}

static void _onGetConfig(AsyncWebServerRequest *request)
{
#ifdef WEB_SECURE
    if (!webAuthenticate(request))
    {
        return request->requestAuthentication(SETTING("espname").c_str());
    }
#endif

    WebSender sender;
    sender.config(request);
}

static void _onSaveConfig(AsyncWebServerRequest *request)
{
#ifdef WEB_SECURE
    if (!webAuthenticate(request))
    {
        return request->requestAuthentication(SETTING("espname").c_str());
    }
#endif

    WebSender sender;
    sender.save_config(request);
}

static void _onInitDb(AsyncWebServerRequest *request)
{
#ifdef WEB_SECURE
    if (!webAuthenticate(request))
    {
        return request->requestAuthentication(SETTING("espname").c_str());
    }
#endif

    Setting.RestoreDefault();

    AsyncResponseStream *response = request->beginResponseStream("text/json");
    response->addHeader("Access-Control-Allow-Origin", "*");
    response->printf("{ \"result\": \"%s\" }", RESP_OK);
    request->send(response);
}

static void _onReset(AsyncWebServerRequest *request)
{
#ifdef WEB_SECURE
    if (!webAuthenticate(request))
    {
        return request->requestAuthentication(SETTING("espname").c_str());
    }
#endif
    request->send(404);

    ESP.restart();
}

static void _onUnlock(AsyncWebServerRequest *request)
{
#ifdef WEB_SECURE
    if (!webAuthenticate(request))
    {
        return request->requestAuthentication(SETTING("espname").c_str());
    }
#endif
    WebSender sender;
    sender.unlockSystem(request);
}

static void _onMqtt(AsyncWebServerRequest *request)
{
#ifdef WEB_SECURE
    if (!webAuthenticate(request))
    {
        return request->requestAuthentication(SETTING("espname").c_str());
    }
#endif
    WebSender sender;
    sender.execute_mqtt(request,_Web_callback);
}

class WebServerClass
{
  public:
    WebServerClass() {}

    void begin()
    {
        // Create server
        uint16_t port = SETTING_I("WEBPORT");
        _server = new AsyncWebServer(port);

        // Rewrites
        _server->rewrite(COMMAND_HOME, COMMAND_HOME_2);

        // 404
        _server->onNotFound([](AsyncWebServerRequest *request) {
            request->send(404);
        });

        _server->on(COMMAND_HOME_2, HTTP_GET, _onHome);
        _server->on(COMMAND_HOME_JSON, HTTP_GET, _onHomeJson);
        _server->on(COMMAND_CONFIG, HTTP_GET, _onGetConfig);
        _server->on(COMMAND_EDIT_CONFIG, HTTP_GET, _onSaveConfig);
        _server->on(COMMAND_RESET, HTTP_GET, _onReset);
        _server->on(COMMAND_INIT_DATABASE, HTTP_GET, _onInitDb);
        _server->on(COMMAND_UNLOCK_STABILITY, HTTP_GET, _onUnlock);
        _server->on(COMMAND_MQTT_SELF_SENDER, HTTP_GET, _onMqtt);
        _server->begin();

#ifdef DEBUG
        DEBUG_MSG("[WEBSERVER] Webserver running on port %u \n", port);
#endif
    }

    void begin(OnWebMqttCallback callback)
    {
        _Web_callback = callback;
        this->begin();
    }
};

WebServerClass WebServer;

#endif
