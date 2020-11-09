#ifndef DDNS_H
#define DDNS_H

#ifdef ESP32
#include "HTTPClient.h"
#else
#include "ESP8266HTTPClient.h"
#endif

#include "plugin_base.h"
#include "plugin_sensor.h"

/****************************************************************
*            
*     DDNS SEND REQUEST TO SERVER 
*
*     Init String:  {[*]/[NAME]/[delay]}
*
***************************************************************/

class ddns : public plugin_base, public plugin_sensor
{
public:
  ddns(const char *init)
      : plugin_sensor()
  {
    std::vector<String> config = splitString(init, PLUGIN_INIT_SEPARATOR_CHAR);
    url = SETTING_P(PSTR("ddnsUrl"));

    if (url.length() > 0 && config.size() == 3)
    {
      name = config[1];
      sensor_delay = atol(config[2].c_str()) * 1000;

      initialized = true;

      OZ_LOG_I_P(PSTR("[DDNS][%s] IS INITIALZED \n"), name.c_str());
      OZ_LOG_I_P(PSTR("[DDNS][%s] URL: %s \n"), name.c_str(), url.c_str());
    }
    else
    {
      OZ_LOG_E_P(PSTR("[DDNS][%s][ERROR] WRONG INITIALZE STRING OR NO DDNS URL \n"), name.c_str());
    }
  }

  void execute_sensor() override
  {
    if (Network.wifiIsConnect())
    {
      HTTPClient http;

      http.begin(url);

      int httpCode = http.GET();
      if (httpCode > 0)
      {
        String payload = http.getString();
        OZ_LOG_V_P(PSTR("[DDNS][%s] Request result: %s \n"), name.c_str(), payload.c_str());
      }

      http.end();
    }
  }

  static const char *ClassName() { return "DDNS"; }

private:
  String url;
  uint16_t send_delay;
};

#endif