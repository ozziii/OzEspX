#ifndef ZE07CO_H
#define ZE07CO_H

#include <HardwareSerial.h>
#include <SoftwareSerial.h>

#include "plugin_base.h"
#include "plugin_sensor.h"


/****************************************************************
*            
*     ZE07-CO carbon monoxide sensor
*
*     Init String:  {[*]/[NAME]/[delay]/[Serial or (Tx_pin-Rx_pin)]}
*
***************************************************************/

class ZE07CO : public plugin_base, public plugin_sensor
{
public:
  ZE07CO(const char *init)
      : plugin_sensor()
  {
    std::vector<String> config = splitString(init, PLUGIN_INIT_SEPARATOR_CHAR);

    if (config.size() >= 4)
    {
      this->name = config[1];
      this->sensor_delay = atol(config[2].c_str()) * 1000;

      std::vector<String> serial_split = splitString(config[3].c_str(), PIN_SEPARATOR_CHAR);
      if (serial_split.size() == 1)
      {
        uint8_t serial_number = atoi(serial_split[0].c_str());
        this->serial = new HardwareSerial(serial_number);
#ifdef DEBUG_INFO
        DEBUG_MSG_P(PSTR("[ZE07CO][%s] USE HARDWARE SERIAL PORT NUMBER %d \n"), this->name.c_str(), serial_number);
#endif
      }
      else
      {
        uint8_t transmitPin = atoi(serial_split[0].c_str());
        uint8_t receivePin = atoi(serial_split[1].c_str());
        this->serial = new SoftwareSerial(receivePin, transmitPin);
#ifdef DEBUG_INFO
        DEBUG_MSG_P(PSTR("[ZE07CO][%s] CREATE SOFTWARE SERIAL Tx: %d, Rx: %d  \n"), this->name.c_str(), transmitPin, receivePin);
#endif
      }

      this->topic_temperature = buildTopic(MQTT_SENSOR_CO);

      initialized = true;

#ifdef DEBUG_LOG
      DEBUG_MSG_P(PSTR("[ZE07CO][%s] IS INITIALZED PIN: %d  DELAY %d \n"), this->name.c_str(), _pin, sensor_delay);

    }
    else
    {

      DEBUG_MSG_P(PSTR("[ZE07CO][%s][ERROR] WRONG INITIALZE STRING \n"), this->name.c_str());
#endif
    }
  }

  void execute_sensor() override
  {
    String b_co;

    Network.send(this->topic_carbon_monoxide.c_str(), b_co.c_str());

#ifdef DEBUG_INFO
    DEBUG_MSG_P(PSTR("[ZE07CO][%s] SEND CO concentation %d ppm  \n"), this->name.c_str(), b_co);
#endif
  }

  static const char *ClassName() { return "ZE07CO"; }



private:
  String topic_carbon_monoxide;
  Stream *serial;

};

#endif