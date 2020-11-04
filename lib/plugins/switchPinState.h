#ifndef SWITCHPINSTATE_H
#define SWITCHPINSTATE_H

#include "plugin_base.h"
#include "plugin_response.h"
#include "plugin_interrupt.h"
#include "plugin_sensor.h"

/******************************************************************************************************************************************
*            
*     CONTROL SWITCH WITH DIFFERENT READ PIN
*
*     Init String:  {1/[NAME]/[action pin]/[read pin]/[delay]=0/[action logic 1:0]=1/[read logic 1:0]=1/[start action pin 1:0]=0/[Pull up 1:0]=0}
*
*********************************************************************************************************************************************/

class switchPinState : public plugin_base, public plugin_interrupt, public plugin_response, public plugin_sensor
{
public:
    switchPinState(const char *init)
    {
        std::vector<String> config = splitString(init, PLUGIN_INIT_SEPARATOR_CHAR);

        if (config.size() >= 4)
        {
            name = config[1];
            _action_pin = atoi(config[2].c_str());
            _read_pin = atoi(config[3].c_str());

            if (config.size() >= 5)
            {
                sensor_delay = atol(config[4].c_str()) * 1000;
            }

            bool action_logic = true;
            if (config.size() >= 6)
            {
                action_logic = atoi(config[5].c_str());
            }

            if (action_logic)
            {
                _action_logic_ON = HIGH;
                _action_logic_OFF = LOW;
            }
            else
            {
                _action_logic_ON = LOW;
                _action_logic_OFF = HIGH;
            }

            bool read_logic = true;
            if (config.size() >= 7)
            {
                read_logic = atoi(config[6].c_str());
            }

            if (read_logic)
            {
                _read_logic_ON = HIGH;
                _read_logic_OFF = LOW;
            }
            else
            {
                _read_logic_ON = LOW;
                _read_logic_OFF = HIGH;
            }

            // HARDWAR CONFIGURATION
            IO.setMode(_action_pin, OUTPUT_MODE);
            if (config.size() >= 8)
            {
                bool action_pin_initial_state = atoi(config[7].c_str());
                IO.writeDigital(_action_pin, action_pin_initial_state);
            }
            else
            {
                IO.writeDigital(_action_pin, LOW);
            }

            // PullUp yes or not
            if (config.size() >= 9)
            {
                if (atoi(config[8].c_str()))
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

            SetInterrups(_read_pin, INTERRUP_MODE_CHANGE);

            // MQTT CONFIGURATION
            topic_action = buildTopic(MQTT_COMMAND_ACTION);
            topic_state = buildTopic(MQTT_COMMAND_STATE);
            Network.subscribe(topic_action.c_str());

            initialized = true;

#ifdef DEBUG_LOG
            DEBUG_MSG_P(PSTR("[SWITCH][%s] CREATE (ACTION PIN: %d) (READ PIN: %d) \n"), name.c_str(), _action_pin, _read_pin);
            DEBUG_MSG_P(PSTR("[SWITCH][%s] SUBSCRIBE TO (%s) \n"), name.c_str(), topic_action.c_str());
#endif
        }
        else
        {
#ifdef DEBUG_ERROR
            DEBUG_MSG_P(PSTR("[RELAY][%s][ERROR] WRONG INITIALZE STRING \n"), name.c_str());
#endif
        }
    }

    bool sendResponse(String topic, String Message) override
    {
        if (topic.equals(topic_action))
        {
            if (Message.equals(MQTT_STATE_OFF))
            {
                if (IO.readDigital(_read_pin) == _read_logic_ON)
                {
                    switchState(_action_logic_OFF);
                }
                return true;
            }

            if (Message.equals(MQTT_STATE_ON))
            {
                if (IO.readDigital(_read_pin) == _read_logic_OFF)
                {
                    switchState(_action_logic_ON);
                }
                return true;
            }

            return true;
        }

        return false;
    }

    virtual void switchState(uint8_t state)
    {
        IO.writeDigital(_action_pin, state);
    }

    virtual bool Interrupt(uint8_t pin) override
    {
        if (_read_pin != pin)
            return false;

        this->state = IO.readDigital(_read_pin) == _read_logic_ON;

        const char *str_state = this->state ? MQTT_STATE_ON : MQTT_STATE_OFF;

        Network.send(topic_state.c_str(), str_state);

        return true;
    }

    virtual void execute_sensor() override
    {
        bool temp_state = IO.readDigital(_read_pin) == _read_logic_ON;

        if (temp_state != state)
        {
            this->state = state;
            const char *str_state = this->state ? MQTT_STATE_ON : MQTT_STATE_OFF;
            Network.send(topic_state.c_str(), str_state);
        }

#ifdef DEBUG_INFO
        DEBUG_MSG_P(PSTR("[RELAY][%s] SEND STATE ( %s ) value %s \n"), name.c_str(), topic_state.c_str(), state);
#endif
    }

    uint8_t _action_pin;
    uint8_t _read_pin;

    uint8_t _action_logic_ON;
    uint8_t _action_logic_OFF;

    uint8_t _read_logic_ON;
    uint8_t _read_logic_OFF;

    String topic_state;
    String topic_action;

    bool state = false;

    static const char *ClassName() { return "SWITCH_PIN_STATE"; }
};

#endif