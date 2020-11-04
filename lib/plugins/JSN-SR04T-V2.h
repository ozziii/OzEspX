#include "configuration.h"

#ifdef PLUGIN_JSNSR04TV2
#ifndef JSNSR04TV2_H
#define JSNSR04TV2_H

#include "plugin_base.h"
#include "plugin_sensor.h"


#include <SoftwareSerial.h>


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

            serial = new SoftwareSerial(
                pins::convertPin(trig_pin),
                pins::convertPin(echo_pin),
                false,
                256);

            serial->begin(9600,SWSERIAL_8N1);

            initialized = true;

#ifdef DEBUG_LOG
            DEBUG_MSG_P(PSTR("[JSN][%s] IS INITIALZED  \n"), name.c_str());
#endif
        }
        else
        {
#ifdef DEBUG_ERROR
            DEBUG_MSG_P(PSTR("[JSN][%s][ERROR] WRONG INITIALZE STRING \n"), name.c_str());
#endif
        }
    }

    void execute_sensor() override
    {
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
#ifdef DEBUG_INFO
            DEBUG_MSG_P(PSTR("[JSN][%s] READ VALUE %d at i : %d \n"), name.c_str(), value, i);
#endif
            i++;
        }
#ifdef DEBUG_INFO
        DEBUG_MSG_P(PSTR("[JSN][%s] DITANCE IS %d mm \n"), name.c_str(), distance);
#endif

    }

    static const char *ClassName() { return "JSN-SR04T-V2"; }

  private:

    SoftwareSerial *serial;

    uint8_t echo_pin;
    uint8_t trig_pin;
};

#endif
#endif