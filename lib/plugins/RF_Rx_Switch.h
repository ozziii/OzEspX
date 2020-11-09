#include <configuration.h>

#ifdef PLUGIN_RADIOFREQUENCY_REVICER

#ifndef RF_Rx_Switch_H
#define RF_Rx_Switch_H

#include "plugin_base.h"
#include "plugin_response.h"
#include "RF_Rx_Switch_lib.h"

/******************************************************************************************************************************************
*            
*     CONTROL SWITCH WITH DIFFERENT READ PIN
*
*     Init String:  {1/[NAME]/[sensor pin]/[switch pin]/[relay logic 1:0]/[valid key]}
*
*********************************************************************************************************************************************/

#define RF_SWITCH_PUMP_RELAY 1000
#define RF_SWITCH_DEBOUNCING_DELAY 1000

static uint8_t rf_rx_switch_pin;
static uint32_t rf_rx_enambleKey;
static unsigned long rf_rx_last_trigger = 0;
static unsigned long rf_rx_switch_interval = 1000;

static void recive_rf(int rf_key)
{

    OZ_LOG_I_P(PSTR("[RF] recive key %d \n"), rf_key);
    if (rf_key == rf_rx_enambleKey)
    {
        if (millis() > rf_rx_last_trigger + rf_rx_switch_interval)
        {
            OZ_LOG_I_P(PSTR("[RF] key is valid \n"));
            IO.pumpDigital(rf_rx_switch_pin, RF_SWITCH_PUMP_RELAY);
            rf_rx_last_trigger = millis();
        }
    }
}

class rf_rx_switch : public plugin_base, public plugin_response
{
public:
    rf_rx_switch(const char *init)
    {
        std::vector<String> config = splitString(init, PLUGIN_INIT_SEPARATOR_CHAR);

        if (config.size() >= 6)
        {
            name = config[1];
            rf_pin = atoi(config[2].c_str());
            rf_rx_switch_pin = atoi(config[3].c_str());
            bool relaylogic = atoi(config[4].c_str());
            rf_rx_enambleKey = atoi(config[5].c_str());

            pins::setMode(rf_rx_switch_pin, OUTPUT_MODE);

            if (relaylogic)
                pins::writeDigital(rf_rx_switch_pin, HIGH);
            else
                pins::writeDigital(rf_rx_switch_pin, LOW);

            mx_rm_5v.enableReceive(pins::convertPin(rf_pin), recive_rf);

            initialized = true;

            OZ_LOG_I_P(PSTR("[RF][%s] CREATE (Sensor pin: %d) (action pin: %d) \n"), name.c_str(), rf_pin, rf_rx_switch_pin);
            OZ_LOG_I_P(PSTR("[RF][%s] SUBSCRIBE TO (%s) \n"), name.c_str(), "null");
        }
        else
        {
            OZ_LOG_E_P(PSTR("[RF][%s][ERROR] WRONG INITIALZE STRING \n"), name.c_str());
        }
    }

    bool sendResponse(String topic, String message)
    {
        return false;
    }

    static const char *ClassName() { return "RF_Rx_Switch"; }

private:
    RCSwitch mx_rm_5v = RCSwitch();
    uint8_t rf_pin;
};

#endif
#endif