#include "vmc_plugin.h"

#ifdef PLUGIN_VMC

static void vmc_switch_direction_cb(uint8_t pin)
{
    IO.switcDigital(pin);
}

vmc_plugin::vmc_plugin(const char *init)
{
    std::vector<String> config = splitString(init, PLUGIN_INIT_SEPARATOR_CHAR);

    if (config.size() == 5)
    {
        name = config[1];
        _enable_pin = atoi(config[2].c_str());
        _pwm_pin = atoi(config[3].c_str());
        _direction_pin = atoi(config[4].c_str());

        topic_state = buildTopic(MQTT_COMMAND_STATE);

        topic_action = buildTopic(MQTT_COMMAND_ACTION);
        Network.subscribe(topic_action.c_str());

        IO.setMode(_enable_pin, OUTPUT_MODE);
        IO.writeDigital(_enable_pin, VMC_ENABLE_OFF);

        IO.setMode(_direction_pin, OUTPUT_MODE);

        IO.setMode(_pwm_pin, PWM_MODE);
        IO.PWM(_pwm_pin, 0);

        initialized = true;

#ifdef DEBUG_LOG
        DEBUG_MSG_P(PSTR("[VMC][%s] CREATE!! (ENABLE PIN: %d, PWM PIN: %d, DIRECTION PIN: %d)\n"),
                    name.c_str(),
                    _enable_pin,
                    _pwm_pin,
                    _direction_pin);
#endif

#ifdef DEBUG_INFO
        DEBUG_MSG_P(PSTR("[VMC][%s] SUBSCRIBE TO (%s) \n"), name.c_str(), topic_action.c_str());
#endif
    }
    else
    {
#ifdef DEBUG_ERROR
        DEBUG_MSG_P(PSTR("[VMC][%s][ERROR] WRONG INITIALZE STRING \n"), name.c_str());
#endif
    }
}

bool vmc_plugin::sendResponse(String topic, String Message)
{
    if (topic.equals(topic_action))
    {
        std::vector<String> payload = splitString(Message.c_str(), VMC_PAYLOAD_SEPARATOR_CHAR);

        if (payload.size() != 3)
            return true;

        if (payload[0].equals(VMC_STATE_AUTO))
        {
            uint16_t delay = atol(payload[2].c_str());

            if (this->_switch_tiker.isActive())
                this->_switch_tiker.changePeriod(delay);
            else
                this->_switch_tiker.attach(delay, vmc_switch_direction_cb, this->_direction_pin);

            IO.writeDigital(this->_direction_pin, VMC_DIRECTION_OUT); //TODO

            uint16_t speed = atol(payload[1].c_str());
            speed = map(speed, 0, 100, 0, 1024);
            IO.PWM(_pwm_pin, speed);

            IO.writeDigital(this->_enable_pin, VMC_ENABLE_ON);

        }

        if (payload[0].equals(VMC_STATE_IN))
        {
            if (this->_switch_tiker.isActive())
                this->_switch_tiker.detach();

            IO.writeDigital(this->_direction_pin, VMC_DIRECTION_IN); //TODO

            uint16_t speed = atol(payload[1].c_str());
            speed = map(speed, 0, 100, 0, 1024);
            IO.PWM(_pwm_pin, speed);

            IO.writeDigital(this->_enable_pin, VMC_ENABLE_ON);;
        }

        if (payload[0].equals(VMC_STATE_OUT))
        {
            if (this->_switch_tiker.isActive())
                this->_switch_tiker.detach();

            IO.writeDigital(this->_direction_pin, VMC_DIRECTION_OUT); //TODO

            uint16_t speed = atol(payload[1].c_str());
            speed = map(speed, 0, 100, 0, 1024);

            IO.PWM(_pwm_pin, speed);

            IO.writeDigital(this->_enable_pin, VMC_ENABLE_ON);
        }

        if (payload[0].equals(VMC_STATE_OFF))
        {
            if (this->_switch_tiker.isActive())
                this->_switch_tiker.detach();

            IO.writeDigital(this->_enable_pin, VMC_ENABLE_OFF);
            IO.PWM(_pwm_pin, 0);
        }

        return true;
    }

    return false;
}

#endif