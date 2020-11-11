/**
 * 
 *      root_topi espnane/name/
 *                                            
 *    HASSIO ------------------------------------------->  SERVER  ----------------------------------------------->   DEVICE
 * 
 *                 espnane/name/adddress/action                             address -> action/message                   |
 *                        message                                                                                       |
 *                                                                                                                      |
 *                                                                                                                      V
 * 
 *    HASSIO <-------------------------------------------  SERVER  <-----------------------------------------------  DEVICE
 * 
 *                 espnane/name/adddress/status                                  address/status/message
 *                        message 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 */ 

#include "configuration.h"

#ifdef PLUGIN_LORATOMQTT

#ifndef LORATOMQTT_H
#define LORATOMQTT_H

#include "plugin_base.h"
#include "plugin_response.h"
#include "plugin_sensor.h"

#include <LoRa_E32.h>
#include <e32cp.h>

#define LORA_UART_DELAY  LOOP_DELAY + 2000

/**
*        
//12/lora/0/3/14/13/21/2

*     LORA TO MQTT
*
*     Init String:  {[*]/[NAME]/AUX/M0/M1/[UART_NUMBER] }
*
*     Topic espname/Lora/NAME/Address/action   => 
*
*     SUBSCRIBE lora/name/#
*   
*/
class LoraToMqtt : public plugin_base, public plugin_response, public plugin_sensor
{

public:
    LoraToMqtt(const char *init);

    bool sendResponse(String topic, String Message) override;

    void execute_sensor() override;

    static const char *ClassName() { return "LORATOMQTT"; }

private:
    String _root_topic;
    LoRa_E32 *_lora;
    e32cp *_protocoll;
    HardwareSerial * _serial;
    uint8_t 
        _uart_number,
        _aux,
        _m1,
        _m0;
};

#endif
#endif