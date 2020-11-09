#ifndef LEDPLUGIN_H
#define LEDPLUGIN_H

#include "plugin_base.h"
#include "plugin_response.h"

/******************************************************************************************************************************************
*            
*     LED AND RGB LED CONTROL
*
*     Init String:  {1/[NAME]/[min 0:1024]/[max 0:1024]/[Wite pin or R-G-B ]/[Dimmer Logic 1:0] = 1/[On-Off pin]?/[ON-Off Logic 1:0] = 1}
*
*********************************************************************************************************************************************/


class led_plugin : public plugin_base, public plugin_response
{
  public:
    led_plugin(const char *init)
    {
        std::vector<String> config = splitString(init, PLUGIN_INIT_SEPARATOR_CHAR);

        if (config.size() >= 5)
        {
            name = config[1];
            min = atoi(config[2].c_str());
            max = atoi(config[3].c_str());

            String std_wite_pin = config[4].c_str();
            if (std_wite_pin.indexOf(PIN_SEPARATOR_CHAR) >= 0)
            {
                std::vector<String> rgbpin = splitString(std_wite_pin.c_str(), PIN_SEPARATOR_CHAR);
                if (rgbpin.size() == 3)
                {
                    red_pin = atoi(rgbpin[0].c_str());
                    pins::setMode(red_pin, PWM_MODE);

                    green_pin = atoi(rgbpin[1].c_str());
                    pins::setMode(green_pin, PWM_MODE);

                    blue_pin = atoi(rgbpin[2].c_str());
                    pins::setMode(blue_pin, PWM_MODE);

                    wite_pin = -1;
                }
                else
                {
                    OZ_LOG_E_P(PSTR("[LED][%s][ERROR] INITIALZATION ERROR WRONG RGB PIN \n"), name.c_str());
                    initialized = false;
                    return;
                }
            }
            else
            {
                wite_pin = atoi(std_wite_pin.c_str());
                pins::setMode(wite_pin, PWM_MODE);
            }

            dimmer_logic = true;
            if (config.size() >= 6)
            {
                dimmer_logic = atoi(config[5].c_str());
                if (!dimmer_logic)
                {
                    uint16_t temp = max;
                    max = min;
                    min = temp;
                }
            }

            onoff_pin = -1;
            if (config.size() >= 7)
            {
                onoff_pin = atoi(config[6].c_str());
                pins::setMode(onoff_pin, OUTPUT_MODE);
            }

            onoff_pin_logic = true;
            if (config.size() >= 8)
            {
                onoff_pin_logic = atoi(config[7].c_str());
            }

            topic_action = this->buildTopic(MQTT_COMMAND_ACTION);
            topic_brightness_action = this->buildTopic(stringjoin(TOPIC_BRIGHTNESS, MQTT_COMMAND_ACTION));
            topic_rgb_action = this->buildTopic(stringjoin(TOPIC_RGB, MQTT_COMMAND_ACTION));

            Network.subscribe(topic_action.c_str());
            Network.subscribe(topic_brightness_action.c_str());
            Network.subscribe(topic_rgb_action.c_str());

            topic_state = this->buildTopic(MQTT_COMMAND_STATE);
            topic_brightness_state = this->buildTopic(stringjoin(TOPIC_BRIGHTNESS, MQTT_COMMAND_STATE));
            topic_rgb_state = this->buildTopic(stringjoin(TOPIC_RGB, MQTT_COMMAND_STATE));

            IsOn = true;
            brightness = 0;
            red = 255;
            green = 255;
            blue = 255;

            initialized = true;
        }

        if (initialized)
        {
            update_lamp();
            OZ_LOG_I_P(PSTR("[LED][%s] INITIALZATION OK \n"), name.c_str());
            OZ_LOG_I_P(PSTR("[LED][%s] STATE %s \n"), name.c_str(), topic_state.c_str());
            OZ_LOG_I_P(PSTR("[LED][%s] BRIGH %s \n"), name.c_str(), topic_brightness_state.c_str());
            OZ_LOG_I_P(PSTR("[LED][%s] RBG %s \n"), name.c_str(), topic_rgb_state.c_str());
        }
        else
        {
            OZ_LOG_E_P(PSTR("[LED][%s][ERROR] INITIALZATION ERROR \n"), name.c_str());
        }
    }

    bool sendResponse(String topic, String Message) override
    {

        if (topic.equals(topic_brightness_action))
        {
            if (IsOn)
            {
                int temp_brightness = Message.toInt();

                brightness = constrain(temp_brightness, 0, 100);
                this->update_lamp();
                this->publish_brightness();
            }
            else
                this->publish_state();

            return true;
        }

        if (topic.equals(topic_rgb_action))
        {
            if (IsOn)
            {
                std::vector<String> rgb = splitString(Message.c_str(), ',');
                if (rgb.size() == 3)
                {
                    red = constrain(rgb[0].toInt(), 0, 255);
                    green = constrain(rgb[1].toInt(), 0, 255);
                    blue = constrain(rgb[2].toInt(), 0, 255);
                    this->update_lamp();
                    this->publish_rgb();
                }
            }
            else
                this->publish_state();

            return true;
        }

        if (topic.equals(topic_action))
        {
            if (Message.equals(MQTT_STATE_ON) && !IsOn)
                this->turnLamp(true);

            if (Message.equals(MQTT_STATE_OFF) && IsOn)
                this->turnLamp(false);

            this->publish_state();
            return true;
        }

        return false;
    }

    static const char *ClassName() { return "LED DIMMER"; }

  private:
    uint16_t max, min;
    int8_t wite_pin, red_pin, green_pin, blue_pin, onoff_pin;
    uint8_t brightness, red, green, blue;
    bool IsOn, dimmer_logic, onoff_pin_logic;

    String topic_state;
    String topic_action;
    String topic_brightness_state;
    String topic_brightness_action;
    String topic_rgb_state;
    String topic_rgb_action;

    char m_msg_buffer[15];

    void publish_state()
    {
        const char *state = IsOn ? MQTT_STATE_ON : MQTT_STATE_OFF;
        Network.send(topic_state.c_str(), state);
    }

    void publish_brightness()
    {
        snprintf(m_msg_buffer, 15, "%d", brightness);
        Network.send(topic_brightness_state.c_str(), m_msg_buffer);
    }

    void publish_rgb()
    {
        snprintf(m_msg_buffer, 15, "%d,%d,%d", red, green, blue);
        Network.send(topic_rgb_state.c_str(), m_msg_buffer);
    }

    void turnLamp(bool state)
    {
        if (state)
        {
            // TURN ON LAMP
            if (onoff_pin >= 0)
                pins::writeDigital(onoff_pin, onoff_pin_logic);

            update_lamp();
        }
        else
        {
            // TURN OFF LAMP
            if (onoff_pin >= 0)
                pins::PWM(onoff_pin, 0);
            //pins::writeDigital(onoff_pin, !onoff_pin_logic);
            if (wite_pin >= 0)
                pins::PWM(wite_pin, 0);
            //pins::writeDigital(wite_pin, !dimmer_logic);
            if (red_pin >= 0)
                pins::PWM(red_pin, 0);
            //pins::writeDigital(red_pin, !dimmer_logic);
            if (blue_pin >= 0)
                pins::PWM(blue_pin, 0);
            //pins::writeDigital(blue_pin, !dimmer_logic);
            if (green_pin >= 0)
                pins::PWM(green_pin, 0);
            //pins::writeDigital(green_pin, !dimmer_logic);
        }

        IsOn = state;
    }

    void update_lamp()
    {
        if (wite_pin < 0)
        {
            uint16_t p_bright = map(brightness, 0, 100, min, max);
            pins::PWM(red_pin, map(red, 0, 255, min, p_bright));
            pins::PWM(green_pin, map(green, 0, 255, min, p_bright));
            pins::PWM(blue_pin, map(blue, 0, 255, min, p_bright));
        }
        else
        {
            pins::PWM(wite_pin, map(brightness, 0, 100, min, max));
        }
    }
};
#endif