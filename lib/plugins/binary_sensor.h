#ifndef BINARIY_SENSOR_H
#define BINARIY_SENSOR_H

#include "plugin_base.h"
#include "plugin_interrupt.h"
#include "plugin_sensor.h"

/********************************************************************************************
*            
*     BISTABLE RELAY PIN STATE
*
*     Init String:  {[*]/[NAME]/[read pin]/[delay (s)]=0/[read logic 1:0]=1/[Pull up 1:0]=0}
*
********************************************************************************************/

class binary_sensor : public plugin_base, public plugin_interrupt, public plugin_sensor
{
public:
    binary_sensor(const char *init)
    {
        std::vector<String> config = splitString(init, PLUGIN_INIT_SEPARATOR_CHAR);

        if (config.size() >= 3)
        {
            name = config[1];
            _read_pin = atoi(config[2].c_str());

            if (config.size() >= 4)
            {
                sensor_delay = atol(config[3].c_str()) * 1000;
            }
            else
            {
                sensor_delay = 0;
            }

            // SET READ LOGIC
            _read_logic_ON = HIGH;
            if (config.size() >= 5)
            {
                bool read_logic = atoi(config[4].c_str());
                if (!read_logic)
                {
                    _read_logic_ON = LOW;
                }
            }

            // SET PULL UP BY INITI STRING
            if (config.size() >= 6)
            {
                if (atoi(config[5].c_str()))
                {
                    IO.setMode(_read_pin, INPUT_PULLUP_MODE);
                }
                else
                {
                    IO.setMode(_read_pin, INPUT_MODE);
                }
            }
            else
            {
                IO.setMode(_read_pin, INPUT_MODE);
            }

            if (sensor_delay == 0)
            {
                // SET INTERRUPT
                SetInterrups(_read_pin, INTERRUP_MODE_CHANGE);
            }

            // MQTT CONFIGURATION
            topic_state = buildTopic(MQTT_COMMAND_STATE);

            initialized = true;

            OZ_LOG_I_P(PSTR("[BINARIY_SENSOR][%s] CREATE (READ PIN: %d) \n"), name.c_str(), _read_pin);
        }
        else
        {
            OZ_LOG_E_P(PSTR("[BINARIY_SENSOR][%s][ERROR] WRONG INITIALZE STRING \n"), name.c_str());
        }
    }

    virtual bool Interrupt(uint8_t pin) override
    {
        if (_read_pin != pin)
            return false;

        const char *state = IO.readDigital(_read_pin) == _read_logic_ON ? MQTT_STATE_ON : MQTT_STATE_OFF;
        Network.send(topic_state.c_str(), state);
        return true;
    }

    virtual void execute_sensor() override
    {
        bool new_state = IO.readDigital(_read_pin) == _read_logic_ON;

        if (new_state != this->state)
        {
            this->state = new_state;
            const char *str_state = this->state ? MQTT_STATE_ON : MQTT_STATE_OFF;
            Network.send(topic_state.c_str(), str_state);
        }
    }

    static const char *ClassName() { return "BINARIY_SENSOR"; }

protected:
    uint8_t _read_pin;
    uint8_t _read_logic_ON;

    bool state = false;

    String topic_state;
};

#endif