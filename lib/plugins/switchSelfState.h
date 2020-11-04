#ifndef SWITCHSELFSTATE_H
#define SWITCHSELFSTATE_H

#include "plugin_base.h"
#include "plugin_response.h"
#include "plugin_sensor.h"

/****************************************************************
*            
*     SWITCH INTERNAL VARIABLE STATE
*
*     Init String:  {1/[NAME]/[action pin]/[delay]=0/[action logic 1:0]=1/[start action pin 1:0]=0}
*
***************************************************************/

class switchSelfState : public plugin_base, public plugin_response, public plugin_sensor
{
  public:
    switchSelfState(const char *init)
    {
        std::vector<String> config = splitString(init, PLUGIN_INIT_SEPARATOR_CHAR);

        if (config.size() >= 3)
        {
            name = config[1];
            _action_pin = atoi(config[2].c_str());

            if (config.size() >= 4)
            {
                sensor_delay = atol(config[3].c_str()) * 1000;
            }

            bool action_logic = true;
            if (config.size() >= 5)
            {
                action_logic = atoi(config[4].c_str());
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

            // HARDWAR CONFIGURATION
            IO.setMode(_action_pin, OUTPUT_MODE);
            if (config.size() >= 6)
            {
                bool read_pin_initial_state = atoi(config[5].c_str());
                IO.writeDigital(_action_pin,read_pin_initial_state);
            }
            else
            {
                IO.writeDigital(_action_pin,LOW);
            }

            // MQTT CONFIGURATION
            topic_action = buildTopic(MQTT_COMMAND_ACTION);
            topic_state = buildTopic(MQTT_COMMAND_STATE);
            Network.subscribe(topic_action.c_str());

            initialized = true;

#ifdef DEBUG_LOG
            DEBUG_MSG_P(PSTR("[SWITCH][%s] CREATE!! (ACTION PIN: %d)\n"), name.c_str(), _action_pin);
#endif
#ifdef DEBUG_INFO
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
                if (state)
                {
                    switchState(_action_logic_OFF);
                    state = false;
                    Network.send(topic_state.c_str(), MQTT_STATE_OFF);
                }
                return true;
            }

            if (Message.equals(MQTT_STATE_ON))
            {
                if (!state)
                {
                    switchState(_action_logic_ON);
                    state = true;
                    Network.send(topic_state.c_str(), MQTT_STATE_ON);
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

    virtual void execute_sensor() override
    {
        const char *state_str = state ? MQTT_STATE_ON : MQTT_STATE_OFF;
        Network.send(topic_state.c_str(), state_str);
    }

    static const char* ClassName() { return "SWITCH_SELF_STATE";}

  public:
    uint8_t _action_pin;
    uint8_t _action_logic_ON;
    uint8_t _action_logic_OFF;

    String topic_state;
    String topic_action;

    bool state;
};

#endif