#include "configuration.h"
#include "ticker_o.h"

#ifdef PLUGIN_VMC

#ifndef VMC_PLUGIN_H
#define VMC_PLUGIN_H

#include <plugin_base.h>
#include <plugin_response.h>


#define VMC_ENABLE_ON    0
#define VMC_ENABLE_OFF   1

#define VMC_DIRECTION_IN  1
#define VMC_DIRECTION_OUT  0


/****************************************************************
*            
*     Ventilazione Meccanica Controllata 
*
*     Init String:  {[*]/[NAME]/[ENABLE]/[PWM]/[DIRECTION]}
*
***************************************************************/
class vmc_plugin : public plugin_base, public plugin_response
{
public:
    vmc_plugin(const char * init);

    bool sendResponse(String topic, String Message) override;

    static const char *ClassName() { return "VMC"; }

private:
    // PIN DEFINITION
    uint8_t _enable_pin, _pwm_pin, _direction_pin;

    // SCWITCH DIRECTION THREAD
    ticker_o _switch_tiker;

    // mqtt topic
    String topic_state, topic_action;

};

#endif
#endif