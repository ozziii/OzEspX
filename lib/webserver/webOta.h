#include "configuration.h"


#ifdef OTA
#ifndef WEB_OTA_H
#define WEB_OTA_H

#include <ESPAsyncWebServer.h>
#include <Update.h>
#include "ticker_o.h"
#include "static.h"

static ticker_o ota_reboot_tiker;
void static ota_reboot_tiker_task(){ ESP.restart(); }

void WebOtaGet(AsyncWebServerRequest *request)
{
    AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", upload_image , upload_image_len);
    response->addHeader("Content-Encoding", "gzip");
    response->addHeader("X-XSS-Protection", "1; mode=block");
    response->addHeader("X-Content-Type-Options", "nosniff");
    response->addHeader("X-Frame-Options", "deny");
    response->addHeader("Access-Control-Allow-Origin", "*");
    request->send(response);
}

void WebOtaUploadEnd(AsyncWebServerRequest *request)
{
    // the request handler is triggered after the upload has finished...
    // create the response, add header, and send response
    AsyncWebServerResponse *response = request->beginResponse((Update.hasError()) ? 500 : 200, "text/plain", (Update.hasError()) ? RESP_ERROE : RESP_OK);
    response->addHeader("Connection", "close");
    response->addHeader("Access-Control-Allow-Origin", "*");
    request->send(response);
    
    if(!Update.hasError())
    {
        ota_reboot_tiker.once_ms(500,ota_reboot_tiker_task);
    }
    
}

static const void WebOtaUpload(AsyncWebServerRequest *request, const String &filename, size_t index, uint8_t *data, size_t len, bool final)
{
    if (!index)
    {
#if defined(ESP8266)
        size_t content_len = request->contentLength();
        int cmd = (filename.indexOf("spiffs") > -1) ? U_FS : U_FLASH;
        Update.runAsync(true);
        if (!Update.begin(content_len, cmd))
        { // Start with max available size
#elif defined(ESP32)
        int cmd = (filename.indexOf("spiffs") > -1) ? U_SPIFFS : U_FLASH;
        if (!Update.begin(UPDATE_SIZE_UNKNOWN, cmd))
        { // Start with max available size
#endif
            Update.printError(Serial);
        }
    }

    // Write chunked data to the free sketch space
    if (Update.write(data, len) != len)
    {
        Update.printError(Serial);
    }

    if (final)
    { // if the final flag is set then this is the last frame of data
        if (Update.end(true))
        { //true to set the size to the current progress
        }

        Update.printError(Serial);
    }
}

#endif
#endif