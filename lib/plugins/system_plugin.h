#ifndef SYSTEM_PLUGIN_H
#define SYSTEM_PLUGIN_H

#include "plugin_base.h"
#include "plugin_sensor.h"
#include "plugin_response.h"

/******************************************************************************************************************************************
*            
*     INTERNAL PLUGIN FOR SYSTEM NOT INIZIALIZE STRING     
*
*********************************************************************************************************************************************/

class system_plugin : public plugin_base, public plugin_response, public plugin_sensor
{
  public:
    system_plugin()
    {
        sensor_delay = SYSTEM_REBOOT_TIME * 1000;

        //TODO CREATE ALIVE TOPIC
    }

    void execute_sensor() override
    {
        
    }

    bool sendResponse(String topic, String Message) override
    {
        return false;
    }
};

#endif