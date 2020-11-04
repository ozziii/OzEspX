
#ifndef EXECUTOR_H
#define EXECUTOR_H

#include <Arduino.h>
#include <vector>

#include <configuration.h>
#include <debug_o.h>
#include <setting.h>
#include <interrupt.h>
#include <pins.h>
#include <ticker_o.h>

#include <plugins.h>

#define SIGLE_ACTION_PER_PLUGIN

// static function call by  tiker of sensor
static void executeSensor(plugin_sensor *sensor)
{
    sensor->execute_sensor();
}

#ifdef ESP32
// No interrup loop for esp32 use query
static void InterruptTask(void *parameter)
{
    std::vector<plugin_interrupt *> _interrups = *((std::vector<plugin_interrupt *> *)parameter);

    while (true)
    {
        // BLOCKING FUNCTION: WAIT FOR INTERRUP
        int8_t interruptPin = Interrupts.Get();
#ifdef DEBUG_INFO
        DEBUG_MSG_P(PSTR("[EXEC] INTERRUPS PIN ( %d ) \n"), interruptPin);
#endif
        for (uint8_t i = 0; i < _interrups.size(); i++)
        {
            if (_interrups[i]->Interrupt(interruptPin))
            {
                break;
            }
        }
    }
}
#endif

class executor
{
  private:
    std::vector<ticker_o> _tiker_sensors;
    std::vector<plugin_sensor *> _sensors;
    std::vector<plugin_interrupt *> _interrups;
    std::vector<plugin_response *> _responses;

  public:
    executor() {}

    void begin()
    {
        //***************  SYSTEM PLUGIN *******************
        //system_plugin *sys = new system_plugin();
        //_sensors.push_back(sys);
        //_responses.push_back(sys);

        //*************** INITI PLUGIN ******************
        for (uint8_t i = 0; i < MAX_PLUGIN; i++)
        {
            String initiString = SETTING_P(PSTR("ITEM%d"), i);
            plugins_o::createPlugIn(initiString, &_sensors, &_interrups, &_responses);
        }

        //**************  start sensor tiker thread ***************************
        for (uint8_t i = 0; i < _sensors.size(); i++)
        {
            if (_sensors[i]->sensor_delay > MIN_SENSOR_DELAY)
            {
                ticker_o tik;
                tik.attach_ms(_sensors[i]->sensor_delay, executeSensor, _sensors[i]);
                _tiker_sensors.push_back(tik);
            }
        }

        //**************  IF ESP32 CREATE INTERRUT PROCESS thread ***************************
#ifdef ESP32

        xTaskCreate(
            InterruptTask,               /* Task function. */
            "InterruptTask",             /* String with name of task. */
            10000,                       /* Stack size in words. */
            (void *)&_interrups,         /* Parameter passed as input of the task */
            MAX_INTERRUPT_TASK_PRIORITY, /* Priority of the task. */
            NULL);                       /* Task handle. */

#endif

#ifdef DEBUG_LOG
        DEBUG_MSG_P(PSTR("[EXEC][BEGIN] CREATE  %d INTERRUPS,  %d RESPONSER, %d SENSOR  \n"), _interrups.size(), _responses.size(), _tiker_sensors.size());
#endif
    }

    void loop()
    {
        // NO LOOP FOR ESP32 interrupt is processed in separate thread
#ifndef ESP32
        for (int8_t interruptPin = Interrupts.Get(); interruptPin >= 0; interruptPin = Interrupts.Get())
        {
#ifdef DEBUG_INFO
            DEBUG_MSG_P(PSTR("[EXEC] INTERRUPS PIN ( %d ) \n"), interruptPin);
#endif
            for (uint8_t i = 0; i < _interrups.size(); i++)
            {
                if (_interrups[i]->Interrupt(interruptPin))
                {
                    break;
                }
            }
        }
#endif
    }

    void mqttrecive(const char *topic, const char *message)
    {
#ifdef DEBUG_LOG
        DEBUG_MSG_P(PSTR("[EXEC] ARRIVE MQTT TOPIC (%s) MESSAGE (%s) \n"), topic, message);
#endif
        for (uint8_t i = 0; i < _responses.size(); i++)
        {
#ifdef SIGLE_ACTION_PER_PLUGIN
            if (_responses[i]->sendResponse(topic, message))
                return;
#else
            _responses[i]->sendResponse(topic, message)
#endif
        }
    }
};
#endif