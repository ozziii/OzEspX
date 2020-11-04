#ifndef FREERAMPLUGIN_H
#define FREERAMPLUGIN_H

#include "plugin_base.h"
#include "plugin_sensor.h"

/****************************************************************
*            
*     FREE RAM SENDER ( SENSOR ) 
*
*     Init String:  {[*]/[NAME]/[delay]}
*
***************************************************************/

class FreeRAMPlugin : public plugin_base, public plugin_sensor
{
  public:
    FreeRAMPlugin(const char *init)
        : plugin_sensor()
    {
        std::vector<String> config = splitString(init, PLUGIN_INIT_SEPARATOR_CHAR);

        if (config.size() == 3)
        {
            name = config[1];
            sensor_delay = atol(config[2].c_str()) * 1000;
            topic_freeram = buildTopic(MQTT_SENSOR_VALUE);

            initialized = true;

#ifdef DEBUG_LOG
            DEBUG_MSG_P(PSTR("[FREE-RAM][%s] IS INITIALZED DELAY %d \n"), name.c_str(), sensor_delay);
#endif
        }
        else
        {
#ifdef DEBUG_ERROR
            DEBUG_MSG_P(PSTR("[FREE-RAM][%s][ERROR] WRONG INITIALZE STRING \n"), name.c_str());
#endif
        }
    }

    void execute_sensor() override
    {
        char freeRam[10];
        sprintf(freeRam, "%i", ESP.getFreeHeap());
        Network.send(topic_freeram.c_str(), freeRam);
#ifdef DEBUG_INFO
        DEBUG_MSG_P(PSTR("[FREE-RAM][%s] SEND %s \n"), name.c_str(), freeRam);
#endif
    }

    static const char* ClassName() { return "FREE_RAM";}

    String topic_freeram;
};

#endif