#ifndef PLUGINBASE_H
#define PLUGINBASE_H

#include <Arduino.h>
#include <vector>

#include <configuration.h>
#include <pins.h>
#include <interrupt.h>
#include <setting.h>
#include <network.h>
#include <debug_o.h>

#define PLUGIN_BASE_ID 0
#define PLUGIN_SENSOR_ID 1
#define PLUGIN_INTERRUPT_ID 2
#define PLUGIN_SWITCH_ID 3

#define PLUGIN_INIT_SEPARATOR_CHAR 47

class plugin_base
{
  public:
    plugin_base()
    {
    }

    String buildTopic(String action)
    {
        String stret;
        stret += SETTING_P(PSTR("espname"));
        stret += char(MQTT_TOPIC_SEPARATOR_CHAR);
        stret += name;
        stret += char(MQTT_TOPIC_SEPARATOR_CHAR);
        stret += action;
        return stret;
    }

    virtual bool isInitialized()
    {
        return initialized && name.length() > 0;
    }

    String name;
    bool initialized = false;
};

#endif