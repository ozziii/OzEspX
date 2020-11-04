#ifndef PINS_H
#define PINS_H

#include <Arduino.h>
#include <vector>

#include <configuration.h>
#include <ticker_o.h>
#include <debug_o.h>

//Pins mode
#define INPUT_MODE 0x01
#define OUTPUT_MODE 0x02
#define INPUT_PULLUP_MODE 0x03
#define INPUT_PULLDOWN_MODE 0x04
#define ANALOG_MODE 0x05
#define PWM_MODE 0x06

//Interrupt Modes
#define INTERRUP_MODE_RISING 0x01
#define INTERRUP_MODE_FALLING 0x02
#define INTERRUP_MODE_CHANGE 0x03
#define INTERRUP_MODE_ONLOW 0x04
#define INTERRUP_MODE_ONHIGH 0x05

#ifdef ESP32
struct pwm_pair
{
    uint8_t channel;
    uint8_t pin;
};

static std::vector<pwm_pair> pwm_list;
static uint8_t next_channel = 0;

#else
static ticker_o relasePins[MAX_GPIO_NUMBER];
#endif

class pins
{
public:
    static void setMode(uint8_t GPIO, uint8_t mode)
    {
#ifdef ESP32
        if (mode == PWM_MODE)
        {
            if (next_channel < PWM_MAX_CHANNEL)
            {
                ledcSetup(next_channel, PWM_FREQUENCY, PWM_RESOLUTION);
                ledcAttachPin(convertPin(GPIO), next_channel);
                pwm_pair new_pair = {next_channel, GPIO};
                pwm_list.push_back(new_pair);
                next_channel++;
            }
        }
        else
            pinMode(convertPin(GPIO), convertMode(mode));
#else
        pinMode(convertPin(GPIO), convertMode(mode));
#endif
    }

    static int readDigital(uint8_t GPIO)
    {
        return digitalRead(convertPin(GPIO));
    }

    static void writeDigital(uint8_t GPIO, uint8_t value)
    {
        digitalWrite(convertPin(GPIO), value);
    }

    static void switcDigital(uint8_t GPIO)
    {
        bool value = digitalRead(convertPin(GPIO));
        digitalWrite(convertPin(GPIO), !value);
    }

    static void pumpDigital(uint8_t GPIO, uint16_t delay_value)
    {
        if (delay_value == 0)
        {
            delay_value = DFAULT_PUMP_DELAY;
        }

        switcDigital(GPIO);

#ifdef ESP32
        ticker_o relasePins;
        relasePins.once_ms(delay_value, RelasePump, GPIO);
#else
        relasePins[GPIO].once_ms(delay_value, RelasePump, GPIO);
#endif
    }

    unsigned long ipulseIn(uint8_t GPIO, uint8_t state, unsigned long timeout)
    {
        return pulseIn(convertPin(GPIO), state, timeout);
    }

    static uint16_t readAnalog(uint8_t GPIO)
    {
        return analogRead(convertPin(GPIO));
    }

    static void PWM(uint8_t GPIO, uint16_t value)
    {
        if (value > 1024)
            value = 1024;

#ifdef ESP32
        for (int i = 0; i < next_channel; i++)
        {
            if (pwm_list[i].pin == GPIO)
            {
                ledcWrite(pwm_list[i].channel, value);
            }
        }
#else
        analogWrite(convertPin(GPIO), value);
#endif
    }

    uint8_t static convertPin(uint8_t pin)
    {
#ifdef ESP32
        return pin;
#else
        switch (pin)
        {
        case 0:
            return D0;
        case 1:
            return D1;
        case 2:
            return D2;
        case 3:
            return D3;
        case 4:
            return D4;
        case 5:
            return D5;
        case 6:
            return D6;
        case 7:
            return D7;
        case 8:
            return D8;
        case 9:
            return D9;
        case 10:
            return D10;
        }
#endif
    }

    uint8_t static convertMode(uint8_t mode)
    {
#ifdef ESP32
        switch (mode)
        {
        case INPUT_MODE:
            return INPUT;
        case OUTPUT_MODE:
            return OUTPUT;
        case INPUT_PULLUP_MODE:
            return INPUT_PULLUP;
        case INPUT_PULLDOWN_MODE:
            return INPUT_PULLDOWN;
        case ANALOG_MODE:
            return ANALOG;
        case PWM_MODE:
            return OUTPUT;
        }
#else
        switch (mode)
        {
        case INPUT_MODE:
            return INPUT;
        case OUTPUT_MODE:
            return OUTPUT;
        case INPUT_PULLUP_MODE:
            return INPUT_PULLUP;
        case INPUT_PULLDOWN_MODE:
            return INPUT_PULLDOWN_16;
        case ANALOG_MODE:
            return OUTPUT;
        case PWM_MODE:
            return OUTPUT;
        }
#endif

        return 0;
    }

    uint8_t static convertInterrupMode(uint8_t mode)
    {
#ifdef ESP32
        switch (mode)
        {
        case INTERRUP_MODE_RISING:
            return RISING;
        case INTERRUP_MODE_FALLING:
            return FALLING;
        case INTERRUP_MODE_CHANGE:
            return CHANGE;
        case INTERRUP_MODE_ONLOW:
            return ONLOW;
        case INTERRUP_MODE_ONHIGH:
            return ONHIGH;
        }
#else
        switch (mode)
        {
        case INTERRUP_MODE_RISING:
            return RISING;
        case INTERRUP_MODE_FALLING:
            return FALLING;
        case INTERRUP_MODE_CHANGE:
            return CHANGE;
        case INTERRUP_MODE_ONLOW:
            return ONLOW;
        case INTERRUP_MODE_ONHIGH:
            return ONHIGH;
        }
#endif
        return 0;
    }

private:
    static void RelasePump(uint8_t GPIO)
    {
        switcDigital(GPIO);
    }
};

extern pins IO;

#endif