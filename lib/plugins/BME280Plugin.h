#ifndef BME280_H
#define BME280_H

#include "plugin_base.h"
#include "plugin_sensor.h"
#include "Wire.h"

#include <BME280_t.h>
/****************************************************************
*            
*     BME280 TEMPERATURE, PRESSURE AND HUMIDITY SENSOR 
*
*     Init String:  {[*]/[NAME]/[delay]}
*
***************************************************************/

#define BME280_MAX_TEMPERATURE 50
#define BME280_MIN_TEMPERATURE -20
#define BME280_MAX_HUMIDITY 98
#define BME280_MIN_HUMIDITY 0
#define BME280_MAX_PRESSURE 12000
#define BME280_MIN_PRESSURE 100

class BME280Plugin : public plugin_base, public plugin_sensor
{
public:
  BME280Plugin(const char *init)
      : plugin_sensor()
  {
    std::vector<String> config = splitString(init, PLUGIN_INIT_SEPARATOR_CHAR);

    if (config.size() == 3)
    {
      this->name = config[1];
      this->sensor_delay = atol(config[2].c_str()) * 1000;

      this->topic_temperature = buildTopic(MQTT_SENSOR_TEMPERATURE);
      this->topic_humidity = buildTopic(MQTT_SENSOR_HUMIDITY);
      this->topic_pressure = buildTopic(MQTT_SENSOR_PRESSURE);
      this->topic_dewPoint = buildTopic(MQTT_SENSOR_DEWPOINT);

      this->_sensor = new BME280<>();
      Wire.begin(21, 22);

      if (!this->_sensor->begin())
      {
        OZ_LOG_E_P(PSTR("[BME280][%s][ERROR] NO SENSOR FOUND \n"), name.c_str());

        return;
      }

      initialized = true;

      OZ_LOG_I_P(PSTR("[BME280][%s] IS INITIALZED DELAY %d \n"), name.c_str(), sensor_delay);
    }
    else
    {

      OZ_LOG_E_P(PSTR("[BME280][%s][ERROR] WRONG INITIALZE STRING \n"), name.c_str());
    }
  }

  void execute_sensor() override
  {
    this->_sensor->refresh();

    float f_temp = 0;
    char *c_temp = new char[8];

    ///  TEMPERATURA
    f_temp = _sensor->temperature;
    if (BME280_MIN_TEMPERATURE < f_temp && f_temp < BME280_MAX_TEMPERATURE) // Clear wrong data
    {
      dtostrf(f_temp, 6, 2, c_temp);
      Network.send(this->topic_temperature.c_str(), c_temp);
    }

    ///  UMIDITà
    f_temp = this->_sensor->humidity;
    if (BME280_MIN_HUMIDITY < f_temp && f_temp < BME280_MAX_HUMIDITY) // Clear wrong data
    {
      dtostrf(f_temp, 6, 2, c_temp);
      Network.send(this->topic_humidity.c_str(), c_temp);
    }

    ///  PRESSIONE
    f_temp = this->_sensor->pressure / 100.0F;
    if (BME280_MIN_PRESSURE < f_temp && f_temp < BME280_MAX_PRESSURE) // Clear wrong data
    {
      dtostrf(f_temp, 6, 2, c_temp);
      Network.send(this->topic_pressure.c_str(), c_temp);
    }

    ///  RUGIADA
    f_temp = dewPointC(this->_sensor->temperature, this->_sensor->humidity);
    if (BME280_MIN_TEMPERATURE < f_temp && f_temp < BME280_MAX_TEMPERATURE) // Clear wrong data
    {
      dtostrf(f_temp, 6, 2, c_temp);
      Network.send(this->topic_dewPoint.c_str(), c_temp);
    }
    delete[] c_temp;

    OZ_LOG_I_P(PSTR("[BME280][%s] READING DONE \n"), name.c_str());
  }

  static const char *ClassName() { return "BME280"; }

private:
  String topic_temperature;
  String topic_humidity;
  String topic_pressure;
  String topic_dewPoint;

  BME280<> *_sensor;

  float dewPointC(float celsius, float humidity)
  {
    // (1) Saturation Vapor Pressure = ESGG(T)
    double RATIO = 373.15 / (273.15 + celsius);
    double RHS = -7.90298 * (RATIO - 1);
    RHS += 5.02808 * log10(RATIO);
    RHS += -1.3816e-7 * (pow(10, (11.344 * (1 - 1 / RATIO))) - 1);
    RHS += 8.1328e-3 * (pow(10, (-3.49149 * (RATIO - 1))) - 1);
    RHS += log10(1013.246);
    // factor -3 is to adjust units - Vapor Pressure SVP * humidity
    double VP = pow(10, RHS - 3) * humidity;
    // (2) DEWPOINT = F(Vapor Pressure)
    double T = log(VP / 0.61078); // temp var
    return (241.88 * T) / (17.558 - T);
  }
};

#endif