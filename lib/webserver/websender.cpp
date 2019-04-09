#include "websender.h"
#include <static.h>
#include <debug_o.h>
#include <plugins.h>

void WebSender::home(AsyncWebServerRequest *request)
{
    AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", webui_image, webui_image_len);
    response->addHeader("Content-Encoding", "gzip");
    response->addHeader("X-XSS-Protection", "1; mode=block");
    response->addHeader("X-Content-Type-Options", "nosniff");
    response->addHeader("X-Frame-Options", "deny");
    response->addHeader("Access-Control-Allow-Origin", "*");
    request->send(response);
}

void WebSender::homeJson(AsyncWebServerRequest *request)
{
    AsyncResponseStream *response = request->beginResponseStream("text/json");

    DynamicJsonBuffer jsonBuffer;
    JsonObject &root = jsonBuffer.createObject();

    JsonArray &params = root.createNestedArray("parameters");

    JsonObject &param00 = jsonBuffer.createObject();
    param00["key"] = "espname";
    param00["value"] = SETTING_P(PSTR("espname"));
    params.add(param00);

    JsonObject &param01 = jsonBuffer.createObject();
    param01["key"] = "address";
    param01["value"] = WiFi.localIP().toString();
    params.add(param01);

    JsonObject &param02 = jsonBuffer.createObject();
    param02["key"] = "board";
    param02["value"] = DEVICE_NAME;
    params.add(param02);

    JsonObject &param03 = jsonBuffer.createObject();
    param03["key"] = "version";
    param03["value"] = SW_VERSION;
    params.add(param03);

    JsonObject &param04 = jsonBuffer.createObject();
    param04["key"] = "freememory";
    param04["value"] = ESP.getFreeHeap();
    params.add(param04);

#ifdef STABILITY_CHECK_ENABLE
    JsonObject &param05 = jsonBuffer.createObject();
    param05["key"] = "Stabile";
    param05["value"] = EEPROM.read(STABILITY_INDEX) < MAX_CRASH_COUNT;
    params.add(param05);
#endif

    JsonObject &param06 = jsonBuffer.createObject();
    param06["key"] = "Start Time";
    param06["value"] = EEPROM.read(START_TIME);
    params.add(param06);

#ifdef ESP32
    JsonObject &param07 = jsonBuffer.createObject();
    param07["key"] = "Chip temperature";
    param07["value"] = temperatureRead();
    params.add(param07);
#endif

    JsonArray &Jplugins = root.createNestedArray("plugin");

    for (int i = 0; i < plugins_o::size(); i++)
    {
        html_plugin plug = plugins_o::get(i);

        String plug_desc = plug.Description;

        plug_desc.replace("*", String(i));

        JsonObject &Jplugin = jsonBuffer.createObject();

        Jplugin["key"] = plug.name;
        Jplugin["value"] = plug_desc;

        Jplugins.add(Jplugin);
    }

    root.printTo(*response);

    response->addHeader("Access-Control-Allow-Origin", "*");
    request->send(response);
}

void WebSender::config(AsyncWebServerRequest *request)
{
    AsyncResponseStream *response = request->beginResponseStream("text/json");

    DynamicJsonBuffer jsonBuffer;
    JsonObject &root = jsonBuffer.createObject();
    JsonArray &data = root.createNestedArray("setting");

    for (int i = 0; i < Parameters_Size; i++)
    {
        Parameter item = Get_CONFIG_P(&Parameters[i]);
        String config_value = Setting.Get(item.key);
        JsonObject &setting = jsonBuffer.createObject();
        setting["key"] = item.key;
        setting["value"] = config_value;
        setting["size"] = item.size;
        data.add(setting);
    }

    root.printTo(*response);

    response->addHeader("Access-Control-Allow-Origin", "*");
    request->send(response);
}

void WebSender::save_config(AsyncWebServerRequest *request)
{
    AsyncResponseStream *response = request->beginResponseStream("text/json");
    response->addHeader("Access-Control-Allow-Origin", "*");

    AsyncWebParameter *pin_key_param = request->getParam(PARAM_NAME);
    AsyncWebParameter *pin_value_param = request->getParam(PARAM_VALUE);

    if (pin_key_param == nullptr || pin_key_param->value().length() == 0)
    {
        response->printf("{ \"result\": \"%s\" }", RESP_WRONG_PARAMETER);
        request->send(response);
        return;
    }

    if (pin_value_param == nullptr)
    {
        response->printf("{ \"result\": \"%s\" }", RESP_WRONG_PARAMETER);
        request->send(response);
        return;
    }

    String value = pin_value_param->value();

    // replace $ with &
    value.replace(36, 38);

    bool result = Setting.Update(pin_key_param->value(), value);

    const char *ret = result ? RESP_OK : RESP_ERROE;

    response->printf("{ \"result\": \"%s\" }", ret);
    request->send(response);
    return;
}

void WebSender::unlockSystem(AsyncWebServerRequest *request)
{
    AsyncResponseStream *response = request->beginResponseStream("text/json");
    response->addHeader("Access-Control-Allow-Origin", "*");

    AsyncWebParameter *value_param = request->getParam(PARAM_VALUE);

    if (value_param == nullptr)
    {
        response->printf("{ \"result\": \"%s\" }", RESP_WRONG_PARAMETER);
        request->send(response);
        return;
    }

    uint8_t byte_Value = atoi(value_param->value().c_str());

    EEPROM.write(STABILITY_INDEX, byte_Value);
    bool result = EEPROM.commit();

    const char *ret = result ? RESP_OK : RESP_ERROE;

    response->printf("{ \"result\": \"%s\" }", ret);
    request->send(response);
    return;
}

void WebSender::execute_mqtt(AsyncWebServerRequest *request,OnWebMqttCallback callback)
{
    AsyncResponseStream *response = request->beginResponseStream("text/json");
    response->addHeader("Access-Control-Allow-Origin", "*");

    AsyncWebParameter *topic_param = request->getParam(PARAM_TOPIC);
    if (topic_param == nullptr || topic_param->value().length() == 0)
    {
        response->printf("{ \"result\": \"%s\" }", RESP_WRONG_PARAMETER);
        request->send(response);
        return;
    }

    AsyncWebParameter *payload_param = request->getParam(PARAM_MESSAGE);
    if (payload_param == nullptr || payload_param->value().length() == 0)
    {
        response->printf("{ \"result\": \"%s\" }", RESP_WRONG_PARAMETER);
        request->send(response);
        return;
    }

    String topic = topic_param->value();
    String payload = payload_param->value();

    char topic_buff[topic.length()+1];
    char payload_buff[payload.length()+1];

    topic.toCharArray(topic_buff,topic.length()+1);  
    payload.toCharArray(payload_buff,payload.length()+1);  

    callback(topic_buff,payload_buff,payload.length()+1);

    free(topic_buff);
    free(payload_buff);

    response->printf("{ \"result\": \"%s\" }", RESP_OK);
    request->send(response);
    return;
}