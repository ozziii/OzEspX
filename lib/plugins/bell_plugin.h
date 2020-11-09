#ifndef BELL_PLUGIN_H
#define BELL_PLUGIN_H

#include "plugin_base.h"
#include "plugin_response.h"

#define BELL_COMMA_SEPARATOR_CHAR 44

/******************************************************************************************************************************************
*            
*     DIGITAL PIN PLUGIN
*
*     Init String:  {#/[NAME]/[action pin]/[action logic 1:0]=1/[start pin 1:0]=0}
*
*     MQTT COMMAND   ON  |   OFF |    [ON time/10 ms],[OFF time/10 ms],[repetitions]
*
*********************************************************************************************************************************************/
class bell_plugin : public plugin_base, public plugin_response
{
public:
    bell_plugin(const char *init)
    {
        std::vector<String> config = splitString(init, PLUGIN_INIT_SEPARATOR_CHAR);

        if (config.size() >= 3)
        {
            name = config[1];
            this->_action_pin = atoi(config[2].c_str());

            bool action_logic = true;
            if (config.size() >= 4)
            {
                action_logic = atoi(config[3].c_str());
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
            IO.setMode(this->_action_pin, OUTPUT_MODE);
            if (config.size() >= 5)
            {
                bool action_pin_initial_state = atoi(config[4].c_str());
                IO.writeDigital(this->_action_pin, action_pin_initial_state);
            }
            else
            {
                IO.writeDigital(this->_action_pin, LOW);
            }

            // MQTT CONFIGURATION
            topic_state = buildTopic(MQTT_COMMAND_STATE);

            topic_action = buildTopic(MQTT_COMMAND_ACTION);
            Network.subscribe(topic_action.c_str());

            initialized = true;

            OZ_LOG_I_P(PSTR("[BELL][%s] CREATE (ACTION PIN: %d) \n"), name.c_str(), _action_pin);
            OZ_LOG_I_P(PSTR("[BELL][%s] SUBSCRIBE TO (%s) \n"), name.c_str(), topic_action.c_str());
        }
        else
        {
            OZ_LOG_E_P(PSTR("[BELL][%s][ERROR] WRONG INITIALZE STRING \n"), name.c_str());
        }
    }

    bool sendResponse(String topic, String Message) override
    {
        if (topic.equals(topic_action))
        {
            if (Message.equals(MQTT_STATE_OFF))
            {
                if (IO.readDigital(this->_action_pin) == _action_logic_ON)
                {
                    switchState(_action_logic_OFF);
                }
                return true;
            }

            if (Message.equals(MQTT_STATE_ON))
            {
                if (IO.readDigital(this->_action_pin) == _action_logic_OFF)
                {
                    switchState(_action_logic_ON);
                }
                return true;
            }

            std::vector<String> command = splitString(Message.c_str(), BELL_COMMA_SEPARATOR_CHAR);
            if (command.size() == 3)
            {
                uint8_t run = atoi(command[0].c_str());
                uint8_t stop = atoi(command[1].c_str());
                uint8_t times = atoi(command[2].c_str());

                if (run > 0 && stop > 0 && times > 0)
                {
                    OZ_LOG_I_P(PSTR("[BELL][INFO] Begin times: %d | stop: %d | run: %d | pin: %d \n"), times, stop, run, _action_pin);

                    uint32_t parameter = (times << 24) + (stop << 16) + (run << 8) + this->_action_pin;

                    this->bell_task.once_ms(10, run_bell, parameter);
                }
                return true;
            }

            uint8_t seconds = atoi(Message.c_str());
            if (seconds > 0)
            {
                uint32_t parameter = (1 << 24) + (0 << 16) + (seconds << 8) + this->_action_pin;

                this->bell_task.once_ms(10, run_bell, parameter);
            }

            return true;
        }

        return false;
    }

    static const char *ClassName() { return "BELL_PLUGIN"; }

private:
    virtual void switchState(uint8_t state)
    {
        IO.writeDigital(this->_action_pin, state);
    }

    uint8_t _action_logic_ON;
    uint8_t _action_logic_OFF;

    String topic_state;
    String topic_action;

    uint8_t _action_pin;

    ticker_o bell_task;

    static void run_bell(uint32_t command)
    {
        uint8_t *vp = (uint8_t *)&command;

        uint8_t times = vp[3];
        uint8_t stop = vp[2];
        uint8_t run = vp[1];
        uint8_t action_pin = vp[0];


        OZ_LOG_V_P(PSTR("[BELL][INFO]  times: %d | stop: %d | run: %d | pin: %d \n"), times, stop, run, action_pin);


        for (int i = 0; i < times; i++)
        {
            IO.switcDigital(action_pin);
            delay(run * 10);
            IO.switcDigital(action_pin);
            delay(stop * 10);
        }
    }
};

#endif