#ifndef PLUGINS_o_H
#define PLUGINS_o_H

#include <Arduino.h>
#include <vector>

#include <configuration.h>

#include <plugin_sensor.h>
#include <plugin_interrupt.h>
#include <plugin_response.h>

#include "freeRam.h"
#include "ddns.h"
#include "DHT22.h"
#include "switchPinState.h"
#include "bistableRelayPinState.h"
#include "switchSelfState.h"
#include "led_plugin.h"
#include "bistableRelayPinStateNI.h"
#include "binary_sensor.h"


#ifdef PLUGIN_PZEM004T
#include "PZEM004T_o.h"
#endif

#ifdef PLUGIN_JSNSR04TV2
#include "JSN-SR04T-V2.h"
#endif

#ifdef PLUGIN_RADIOFREQUENCY_REVICER
#include "RF_Rx_Switch.h"
#endif

#ifdef NEXTION
#include "nextion_plugin.h"
#endif


struct html_plugin
{
    const char *name;
    const char *Description;
};

class plugins_o
{
  public:
    static void createPlugIn(
        String initString,
        std::vector<plugin_sensor *> *_sensors,
        std::vector<plugin_interrupt *> *_interrups,
        std::vector<plugin_response *> *_responses);

    static html_plugin get(uint8_t index);

    static uint8_t plugin_list_size();
};

#endif