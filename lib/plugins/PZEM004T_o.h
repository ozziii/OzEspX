#include "configuration.h"

#ifdef PLUGIN_PZEM004T

#ifndef PZEM_004T_H
#define PZEM_004T_H

#include <HardwareSerial.h>
#include <SoftwareSerial.h>
#include <PZEM004T.h>

#include <plugin_base.h>
#include <plugin_sensor.h>

/****************************************************************
*            
*     ALIVE SEND CURRENT MILLIS 
*
*     Init String:  {[*]/[NAME]/[delay]/[Hardware(0):Software(1)]/[Serial or Tx pin]=2/[Rx pin]=0}
*
***************************************************************/
class PZEM004T_o : public plugin_base, public plugin_sensor
{
  public:
    PZEM004T_o(const char *init)
        : plugin_sensor()
    {
        ip = IPAddress(192, 168, 1, 1);
        std::vector<String> config = splitString(init, PLUGIN_INIT_SEPARATOR_CHAR);

        if (config.size() >= 5)
        {
            name = config[1];
            sensor_delay = atol(config[2].c_str()) * 1000;

            topic_current = buildTopic(MQTT_SENSOR_CURRENT);
            topic_voltage = buildTopic(MQTT_SENSOR_VOLTAGE);
            topic_energy = buildTopic(MQTT_SENSOR_ENERGY);
            topic_power = buildTopic(MQTT_SENSOR_POWER);

            // [Hardware(0):Software(1)]
            if (atoi(config[3].c_str()))
            {
#ifdef DEBUG
                DEBUG_MSG_P(PSTR("[PZEM004T][%s] CREATE SOFTWARE SERIAL \n"), name.c_str());
#endif
                // SOFTWARE SERIAL
                if (config.size() == 6)
                {
                    unsigned int TxPin = atoi(config[4].c_str());
                    unsigned int RxPin = atoi(config[5].c_str());
                    device = new PZEM004T(pins::convertPin(RxPin), pins::convertPin(TxPin));
                }
                else
                {
#ifdef DEBUG
                    DEBUG_MSG_P(PSTR("[PZEM004T][%s][ERROR] WRONG INITIALZE STRING \n"), name.c_str());
#endif
                    return;
                }
            }
            else
            {
                // HARDWARE SERIAL
                uint8_t serial = atoi(config[4].c_str());
                _serial = new HardwareSerial(serial); // Use hwserial UART2 at pins IO-16 (RX2) and IO-17 (TX2)
                device = new PZEM004T(_serial);

#ifdef DEBUG
                DEBUG_MSG_P(PSTR("[PZEM004T][%s] USE HARDWARE SERIAL PORT %d \n"), name.c_str(), serial);
#endif
            }

            uint8_t try_time = 0;

            while (!device->setAddress(ip) && try_time < 10)
            {
                try_time++;
                delay(1000);
#ifdef DEBUG
                DEBUG_MSG_P(PSTR("[PZEM004T][%s][ERROR] TRY TO CONNECT %d   \n"), name.c_str(), try_time);
#endif
            }

            if (try_time == 10)
            {
#ifdef DEBUG
                DEBUG_MSG_P(PSTR("[PZEM004T][%s][ERROR] UNABLE TO CONNECT TO SEIAL %d \n"), name.c_str());
#endif
                return;
            }

            initialized = true;

#ifdef DEBUG
            DEBUG_MSG_P(PSTR("[PZEM004T][%s] IS INITIALZED DELAY %d \n"), name.c_str(), sensor_delay);
#endif
        }
        else
        {
#ifdef DEBUG
            DEBUG_MSG_P(PSTR("[PZEM004T][%s][ERROR] WRONG INITIALZE STRING \n"), name.c_str());
#endif
        }
    }

    void execute_sensor() override
    {

        float v = device->voltage(ip);
        if (v > 0.0)
        {
            String temp_string(v);
            Network.send(topic_voltage.c_str(),temp_string.c_str());
        }


        float i = device->current(ip);
        if (i >= 0.0)
        {
            String temp_string(i);
            Network.send(topic_current.c_str(),temp_string.c_str());
        }

        float p = device->power(ip);
        if (p >= 0.0)
        {
            String temp_string(p);
            Network.send(topic_power.c_str(),temp_string.c_str());
        }

        float e = device->energy(ip);
        if (e >= 0.0)
        {
            String temp_string(e);
            Network.send(topic_energy.c_str(),temp_string.c_str());
        }
    }

    static const char *ClassName() { return "PZEM004T"; }

    String topic_current;
    String topic_voltage;
    String topic_energy;
    String topic_power;

    HardwareSerial *_serial = NULL;
    PZEM004T *device = NULL;
    IPAddress ip;
};

#endif
#endif