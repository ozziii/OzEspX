#include "configuration.h"

#ifdef PLUGIN_PZEM004T

#ifndef PZEM_004T_H
#define PZEM_004T_H

#include <HardwareSerial.h>
#include <SoftwareSerial.h>

#define PZEM004TV3

#ifdef PZEM004TV3
#include <PZEM004Tv30.h>
#else
#include <PZEM004T.h>
#endif

#include <plugin_base.h>
#include <plugin_sensor.h>

//#define PZEM004T_SEND_VOLTAGE
//#define PZEM004T_SEND_CURRENT
#define PZEM004T_SEND_POWER
#define PZEM004T_SEND_ENERGY

/****************************************************************
*            
*     PZEM004T  AC main voltage  power sensor 
*
*     Init String:  {[*]/[NAME]/[delay]/[Serial or (Tx_pin-Rx_pin)]}
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

        if (config.size() >= 4)
        {
            name = config[1];
            sensor_delay = atol(config[2].c_str()) * 1000;

            std::vector<String> serial_split = splitString(config[3].c_str(), PIN_SEPARATOR_CHAR);
            if (serial_split.size() == 1)
            {
                uint8_t uart_nr = atoi(serial_split[0].c_str());
                _serial = new HardwareSerial(uart_nr); // Use hwserial UART2 at pins IO-16 (RX2) and IO-17 (TX2)

#ifdef PZEM004TV3
                device = new PZEM004Tv30(_serial);
#else
                device = new PZEM004T(_serial);
#endif

#ifdef DEBUG_LOG
                DEBUG_MSG_P(PSTR("[PZEM004T][%s] USE HARDWARE SERIAL PORT NUMBER %d \n"), name.c_str(), uart_nr);
#endif
            }
            else
            {
                /*
                unsigned int TxPin = atoi(serial_split[0].c_str());
                unsigned int RxPin = atoi(serial_split[1].c_str());
                device = new PZEM004T(pins::convertPin(RxPin), pins::convertPin(TxPin));
#ifdef DEBUG_LOG
                DEBUG_MSG_P(PSTR("[PZEM004T][%s] CREATE SOFTWARE SERIAL Tx: %d, Rx: %d \n"), name.c_str(), TxPin, RxPin);
#endif
                */
            }

            uint8_t try_time = 0;

            while (!device->setAddress(ip) && try_time < 10)
            {
                try_time++;
                delay(1000);
#ifdef DEBUG_ERROR
                DEBUG_MSG_P(PSTR("[PZEM004T][%s][ERROR] TRY TO CONNECT %d   \n"), name.c_str(), try_time);
#endif
            }

            if (try_time == 10)
            {
#ifdef DEBUG_ERROR
                DEBUG_MSG_P(PSTR("[PZEM004T][%s][ERROR] UNABLE TO CONNECT TO SERIAL \n"), name.c_str());
#endif
            }
            else
            {
#ifdef PZEM004T_SEND_VOLTAGE
                topic_voltage = buildTopic(MQTT_SENSOR_VOLTAGE);
#endif
#ifdef PZEM004T_SEND_CURRENT
                topic_current = buildTopic(MQTT_SENSOR_CURRENT);
#endif
#ifdef PZEM004T_SEND_POWER
                topic_power = buildTopic(MQTT_SENSOR_POWER);
#endif
#ifdef PZEM004T_SEND_ENERGY
                topic_energy = buildTopic(MQTT_SENSOR_ENERGY);
#endif

                initialized = true;

#ifdef DEBUG_LOG
                DEBUG_MSG_P(PSTR("[PZEM004T][%s] IS INITIALZED DELAY %d \n"), name.c_str(), sensor_delay);
#endif
            }
        }
        else
        {
#ifdef DEBUG_ERROR
            DEBUG_MSG_P(PSTR("[PZEM004T][%s][ERROR] WRONG INITIALZE STRING \n"), name.c_str());
#endif
        }
    }

    void execute_sensor() override
    {
#ifdef PZEM004T_SEND_VOLTAGE

#ifdef PZEM004TV3
        float v = device->voltage();
#else
        float v = device->voltage(ip);
#endif
        if (v > 0.0)
        {
            String temp_string(v);
            Network.send(topic_voltage.c_str(), temp_string.c_str());
        }
#endif
#ifdef PZEM004T_SEND_CURRENT

#ifdef PZEM004TV3
        float i = device->current();
#else
        float i = device->current(ip);
#endif
        if (i >= 0.0)
        {
            String temp_string(i);
            Network.send(topic_current.c_str(), temp_string.c_str());
        }
#endif
#ifdef PZEM004T_SEND_POWER

#ifdef PZEM004TV3
        float p = device->power();
#else
        float p = device->power(ip);
#endif
        if (p >= 0.0)
        {
            String temp_string(p);
            Network.send(topic_power.c_str(), temp_string.c_str());
        }
#endif
#ifdef PZEM004T_SEND_ENERGY

#ifdef PZEM004TV3
        float e = device->energy();
#else
        float e = device->energy(ip);
#endif
        if (e >= 0.0)
        {
            String temp_string(e);
            Network.send(topic_energy.c_str(), temp_string.c_str());
        }
#endif
    }

    static const char *ClassName() { return "PZEM004T"; }

#ifdef PZEM004T_SEND_VOLTAGE
    String topic_voltage;
#endif
#ifdef PZEM004T_SEND_CURRENT
    String topic_current;
#endif
#ifdef PZEM004T_SEND_POWER
    String topic_power;
#endif
#ifdef PZEM004T_SEND_ENERGY
    String topic_energy;
#endif

    HardwareSerial *_serial = NULL;

#ifdef PZEM004TV3
    PZEM004Tv30 *device = NULL;
#else
    PZEM004T *device = NULL;
#endif

    IPAddress ip;
};

#endif
#endif