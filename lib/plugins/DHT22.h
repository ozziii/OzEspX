#ifndef DHT22_H
#define DHT22_H

#include "plugin_base.h"
#include "plugin_sensor.h"
#include "DHTesp.h"

/****************************************************************
*            
*     DHT22 TEMPERATURE AND HUMIDITY SENSOR 
*
*     Init String:  {[*]/[NAME]/[delay]/[pin]}
*
***************************************************************/


class DHT22Plugin : public plugin_base, public plugin_sensor
{
  public:
    DHT22Plugin(const char *init)
        : plugin_sensor()
    {
        std::vector<String> config = splitString(init, PLUGIN_INIT_SEPARATOR_CHAR);

        if (config.size() == 4)
        {
            name = config[1];
            
            sensor_delay = atol(config[2].c_str()) * 1000;

            uint8_t _pin = atoi(config[3].c_str());

            dht.setup(pins::convertPin(_pin), DHTesp::DHT22);

            topic_temperature = buildTopic(MQTT_SENSOR_TEMPERATURE);
            topic_humidity = buildTopic(MQTT_SENSOR_HUMIDITY);

            initialized = true;

#ifdef DEBUG_LOG
            DEBUG_MSG_P(PSTR("[DHT22][%s] IS INITIALZED PIN: %d  DELAY %d \n"), name.c_str(), _pin, sensor_delay);
#endif
        }
        else
        {
#ifdef DEBUG_ERROR
            DEBUG_MSG_P(PSTR("[DHT22][%s][ERROR] WRONG INITIALZE STRING \n"), name.c_str());
#endif
        }
    }

    void execute_sensor() override
    {
        newValues = dht.getTempAndHumidity();
        if (dht.getStatus() != 0)
        {
#ifdef DEBUG_INFO
            DEBUG_MSG_P(PSTR("[DHT22][%s] ERROR MESSAGE ( %s ) \n"), name.c_str(), dht.getStatusString());
#endif
            return;
        }

        char *b_temp = new char[8];
        char *b_humi = new char[8];

        dtostrf(newValues.temperature, 6, 2, b_temp);
        dtostrf(newValues.humidity, 6, 2, b_humi);

        Network.send(topic_temperature.c_str(), b_temp);
        Network.send(topic_humidity.c_str(), b_humi);

#ifdef DEBUG_INFO
        DEBUG_MSG_P(PSTR("[DHT22][%s] SEND TEM: %s HUM: %s  \n"), name.c_str(), b_temp, b_humi);
#endif

        delete[] b_humi;
        delete[] b_temp;
    }

    static const char* ClassName() { return "DHT22";}

  private:
    DHTesp dht;
    TempAndHumidity newValues;

    String topic_temperature;
    String topic_humidity;
};

#endif