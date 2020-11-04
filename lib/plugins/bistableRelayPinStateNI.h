#ifndef RELAYBISTABLEPINSTATE_NI_H
#define RELAYBISTABLEPINSTATE_NI_H

#include "plugin_base.h"
#include "plugin_response.h"
#include "plugin_sensor.h"

/********************************************************************************************
*            
*     BISTABLE RELAY PIN STATE NO INTERRUPT [B.R.P.S.N.I.]
*
*     Init String:  {[*]/[NAME]/[action pin]/[read pin]/[delay]=0/[action logic 1:0]=1/[read logic 1:0]=1/[start action pin 1:0]=0/[Pull up 1:0]=0}
*
********************************************************************************************/

#define BISTABLE_RELAY_NO_PUMP_DELAY 200

class bistableRelayPinStateNI : public plugin_base, public plugin_response, public plugin_sensor
{
  public:
    bistableRelayPinStateNI(const char *init)
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

            // MQTT CONFIGURATION
            topic_action = buildTopic(MQTT_COMMAND_ACTION);
            topic_state = buildTopic(MQTT_COMMAND_STATE);
            Network.subscribe(topic_action.c_str());

            this->current_state = IO.readDigital(_read_pin);

            initialized = true;

#ifdef DEBUG_LOG
            DEBUG_MSG_P(PSTR("[B.R.P.S.N.I.][%s] CREATE (ACTION PIN: %d) (READ PIN: %d) \n"), name.c_str(), _action_pin, _read_pin);
            DEBUG_MSG_P(PSTR("[B.R.P.S.N.I.][%s] SUBSCRIBE TO (%s) \n"), name.c_str(), topic_action.c_str());
#endif
        }
        else
        {
#ifdef DEBUG_ERROR
            DEBUG_MSG_P(PSTR("[B.R.P.S.N.I.][%s][ERROR] WRONG INITIALZE STRING \n"), name.c_str());
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
                    IO.pumpDigital(_action_pin, BISTABLE_RELAY_NO_PUMP_DELAY);
                }
                return true;
            }

            if (Message.equals(MQTT_STATE_ON))
            {
                if (IO.readDigital(_read_pin) == _read_logic_OFF)
                {
                    IO.pumpDigital(_action_pin, BISTABLE_RELAY_NO_PUMP_DELAY);
                }
                return true;
            }

            return true;
        }

        return false;
    }

    virtual void execute_sensor() override
    {
        bool bstate = IO.readDigital(_read_pin);

        if (bstate != this->current_state)
        {
            this->current_state = bstate;
            const char *state = bstate == _read_logic_ON ? MQTT_STATE_ON : MQTT_STATE_OFF;
            Network.send(topic_state.c_str(), state);
#ifdef DEBUG_INFO
            DEBUG_MSG_P(PSTR("[B.R.P.S.N.I.][%s] STATE HAS CHANGED CURRENT %s \n"), name.c_str(), state);
            DEBUG_MSG_P(PSTR("[B.R.P.S.N.I.][%s] SEND ( %s ) value %s \n"), name.c_str(), topic_state.c_str(), state);
#endif
        }
        else
        {
#ifdef DEBUG_INFO
            DEBUG_MSG_P(PSTR("[B.R.P.S.N.I.][%s] NO STATE CHANGED \n"), name.c_str());
#endif
        }
    }

    static const char *ClassName() { return "BISTABLE_RELAY_NO_INTERRUPT"; }

  private:
    uint8_t _action_pin;
    uint8_t _read_pin;

    uint8_t _action_logic_ON;
    uint8_t _action_logic_OFF;

    uint8_t _read_logic_ON;
    uint8_t _read_logic_OFF;

    String topic_state;
    String topic_action;

    bool current_state;
};

#endif