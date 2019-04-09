#ifndef ALIVE_H
#define ALIVE_H

#include <plugin_base.h>
#include <plugin_sensor.h>

/****************************************************************
*            
*     ALIVE SEND CURRENT MILLIS 
*
*     Init String:  {[*]/[NAME]/[delay]}
*
***************************************************************/
class alive : public plugin_base, public plugin_sensor
{
  public:
    alive(const char *init)
        : plugin_sensor()
    {
        std::vector<String> config = splitString(init, PLUGIN_INIT_SEPARATOR_CHAR);

        if (config.size() == 3)
        {
            name = config[1];
            sensor_delay = atol(config[2].c_str()) * 1000;
            topic_current_millis = buildTopic(MQTT_SENSOR_ALIVE);

            initialized = true;

#ifdef DEBUG
            DEBUG_MSG_P(PSTR("[ALIVE][%s] IS INITIALZED DELAY %d \n"), name.c_str(), sensor_delay);
#endif
        }
        else
        {
#ifdef DEBUG
            DEBUG_MSG_P(PSTR("[ALIVE][%s][ERROR] WRONG INITIALZE STRING \n"), name.c_str());
#endif
        }
    }

    void execute_sensor() override
    {
        char current_millis[10];
        sprintf(current_millis, "%u", millis());
        Network.send(topic_current_millis.c_str(), current_millis);
#ifdef DEBUG
        DEBUG_MSG_P(PSTR("[ALIVE][%s] SEND %s \n"), name.c_str(), current_millis);
#endif
    }

    static const char *ClassName() { return "ALIVE"; }

    String topic_current_millis;
};

#endif