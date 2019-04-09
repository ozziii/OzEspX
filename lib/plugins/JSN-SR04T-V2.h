#ifndef JSNSR04TV2_H
#define JSNSR04TV2_H

#include "plugin_base.h"
#include "plugin_sensor.h"

#ifndef ESP32
#include <SoftwareSerial.h>
#endif

/****************************************************************
*            
*     DISTANCE SENSOR
*
*     Init String:  {[*]/[NAME]/[DELAY]/[TRIG PIN]/[ECHO PIN]}
*
***************************************************************/

class JSNSR04TV2 : public plugin_base, public plugin_sensor
{
  public:
    JSNSR04TV2(const char *init)
        : plugin_sensor()
    {
        std::vector<String> config = splitString(init, PLUGIN_INIT_SEPARATOR_CHAR);

        if (config.size() >= 4)
        {
            name = config[1];
            sensor_delay = atol(config[2].c_str()) * 1000;
            trig_pin = atoi(config[3].c_str());
            echo_pin = atoi(config[4].c_str());
#ifndef ESP32
            serial = new SoftwareSerial(
                pins::convertPin(trig_pin),
                pins::convertPin(echo_pin),
                false,
                256);

            serial->begin(9600);

            // IO.setMode(echo_pin,INPUT_PULLUP_MODE);
            // IO.setMode(trig_pin,OUTPUT_MODE);
            //IO.writeDigital(echo_pin,HIGH);
#endif
            initialized = true;

#ifdef DEBUG
            DEBUG_MSG_P(PSTR("[JSN][%s] IS INITIALZED  \n"), name.c_str());
#endif
        }
        else
        {
#ifdef DEBUG
            DEBUG_MSG_P(PSTR("[JSN][%s][ERROR] WRONG INITIALZE STRING \n"), name.c_str());
#endif
        }
    }

    void execute_sensor() override
    {
#ifndef ESP32
        serial->flush();

        int i = 0;
        int distance = 0;

        while (!serial->available())
        {
        }

        while (serial->available() > 0 && i < 6)
        {
            int value = serial->read();

            if (i == 1)
            {
                distance += value * 256;
            }
            if (i == 2)
            {
                distance += value;
            }
#ifdef DEBUG
            DEBUG_MSG_P(PSTR("[JSN][%s] READ VALUE %d at i : %d \n"), name.c_str(), value, i);
#endif
            i++;
        }
#ifdef DEBUG
        DEBUG_MSG_P(PSTR("[JSN][%s] DITANCE IS %d mm \n"), name.c_str(), distance);
#endif

#endif

        /*
        IO.writeDigital(trig_pin,LOW); // Set the trigger pin to low for 2uS
        delayMicroseconds(2);
        IO.writeDigital(trig_pin,HIGH); // Send a 10uS high to trigger ranging
        delayMicroseconds(10);
        IO.writeDigital(trig_pin,LOW); // Send pin low again
    //26000
        int distance = IO.ipulseIn(echo_pin, HIGH,5000); // Read in times pulse
//int distance = 0;
        distance = distance/58;
        #ifdef DEBUG
            DEBUG_MSG_P(PSTR("[JSN][%s] DITANCE IS %d cm \n"), name.c_str(),distance);
        #endif   
        */
    }

    static const char *ClassName() { return "JSN-SR04T-V2"; }

  private:
#ifndef ESP32
    SoftwareSerial *serial;
#endif
    uint8_t echo_pin;
    uint8_t trig_pin;
};

#endif