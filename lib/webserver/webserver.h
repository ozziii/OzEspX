
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

#ifdef OTA
#include "webOta.h"
#endif

static AsyncWebServer *_server;

static OnWebMqttCallback _Web_callback;

#ifdef WEB_SECURE
static bool webAuthenticate(AsyncWebServerRequest *request)
{
    String UserName = SETTING("WEBUSER");
    String Password = SETTING("WEBPASS");
    return request->authenticate(UserName.c_str(), Password.c_str());
}
#endif

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

static ticker_o web_reboot_tiker;
void static web_reboot_tiker_task() { ESP.restart(); }

static void _onReset(AsyncWebServerRequest *request)
{

#ifdef WEB_SECURE
    if (!webAuthenticate(request))
    {
        return request->requestAuthentication(SETTING("espname").c_str());
    }
#endif
    request->send(404);
    web_reboot_tiker.once(2, web_reboot_tiker_task);
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
    sender.execute_mqtt(request, _Web_callback);
}

#ifdef OTA // OTA UPDATE COMMAND
static void _onOTA_GET(AsyncWebServerRequest *request)
{
#ifdef WEB_SECURE
    if (!webAuthenticate(request))
    {
        return request->requestAuthentication(SETTING("espname").c_str());
    }
#endif
    WebOtaGet(request);
}

static void _onOTA_POST_END(AsyncWebServerRequest *request)
{
#ifdef WEB_SECURE
    if (!webAuthenticate(request))
    {
        return request->requestAuthentication(SETTING("espname").c_str());
    }
#endif
    WebOtaUploadEnd(request);
}

static void _onOTA_POST(AsyncWebServerRequest *request, const String &filename, size_t index, uint8_t *data, size_t len, bool final)
{
#ifdef WEB_SECURE
    if (!webAuthenticate(request))
    {
        return request->requestAuthentication(SETTING("espname").c_str());
    }
#endif
    WebOtaUpload(request, filename, index, data, len, final);
}
#endif

#ifdef DEBUG_WEB_SUPPORT
static void _onDebugJson(AsyncWebServerRequest *request)
{
#ifdef WEB_SECURE
    if (!webAuthenticate(request))
    {
        return request->requestAuthentication(SETTING("espname").c_str());
    }
#endif

    WebSender sender;
    sender.DebugJson(request);
}
#endif

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

#ifdef DEBUG_WEB_SUPPORT
        _server->on(COMMAND_DEBUG_JSON, HTTP_GET, _onDebugJson);
#endif

#ifdef OTA // OTA UPDATE COMMAND
        _server->on(COMMAND_OTA_UPDATYE, HTTP_GET, _onOTA_GET);
        _server->on(COMMAND_OTA_UPDATYE, HTTP_POST, _onOTA_POST_END, _onOTA_POST);
#endif

        _server->begin();

        OZ_LOG_I_P(PSTR("[WEBSERVER] Webserver running on port %u \n"), port);
    }

    void begin(OnWebMqttCallback callback)
    {
        _Web_callback = callback;
        this->begin();
    }
};

WebServerClass WebServer;

#endif
