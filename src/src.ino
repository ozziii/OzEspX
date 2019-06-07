#include <Arduino.h>

#include <configuration.h>
#include <system_o.h>
#include <network.h>
#include <webserver.h>
#include <executor.h>

static system_o System;
static executor Program;

#ifdef ESP32 // IF ESP32 Delay server start
ticker_o web_tiker;
void web_callback()
{
    WebServer.begin(callback);
}
#endif

void callback(char *p_topic, char *p_payload, size_t p_length)
{
    char message[p_length + 1];
    strlcpy(message, (char *)p_payload, p_length + 1);
    Program.mqttrecive(p_topic, message);
}

void setup()
{
#ifdef DEBUG
    debug::debugBegin();
#endif


    // Initialize EEPROM setting string
    Setting.begin();

#ifdef FORCE_INIT_SETTING
    Setting.RestoreDefault();
#else
    Setting.InitSettings();
#endif

    System.begin();

    if (System.checkSystemStability())
    {
        // Initialize plug-in and start execution
        Program.begin();
    }

    // Connect to WiFi and MQTT server
    Network.begin(callback);


    // Initialize WebServer
#ifdef ESP32 // IF ESP32 Delay server start
    web_tiker.once(WEBSERVER_START_DELAY, web_callback);
#else
    WebServer.begin(callback);
#endif
              
    
}

void loop()
{
    Network.loop();
    Program.loop();
    // Power consumation delay
    System.loop();
}
