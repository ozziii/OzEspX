#include "configuration.h"

#ifdef PLUGIN_I2S2UDP

#ifndef I2S2UDP_H
#define I2S2UDP_H

#include <plugin_base.h>
#include <plugin_sensor.h>

#include "network.h"
#include "driver/i2s.h"


/****************************************************************
*            
*     i2s -> network 
*
*     Init String:  {[*]/[NAME]/[BCK]/[RLC]/[DIN]}
*
***************************************************************/
class i2s2udp : public plugin_base, public plugin_sensor
{
public:
    i2s2udp(const char * init);

    void execute_sensor() override;

    static const char *ClassName() { return "I2S to UPD"; }

private:
    xTaskHandle i2s_tx_task_handle = NULL;
    
    IPAddress  multiacst_address;
    
    uint8_t _bck_pin, _rlc_pin, _din_pin;

    bool _udp_init = false;
    bool _i2s_init = false;

    bool _serial_init = false;

    void init_i2s();

    void stop_i2s();
};

#endif
#endif