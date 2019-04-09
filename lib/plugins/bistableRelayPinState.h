#ifndef RELAYBISTABLEPINSTATE_H
#define RELAYBISTABLEPINSTATE_H

#include "switchPinState.h"

/********************************************************************************************
*            
*     BISTABLE RELAY PIN STATE
*
*     Init String:  {[*]/[NAME]/[action pin]/[read pin]/[delay]=0/[action logic 1:0]=1/[read logic 1:0]=1/[start action pin 1:0]=0/[Pull up 1:0]=0}
*
********************************************************************************************/

#define BISTABLE_RELAY_PUMP_DELAY 500

class bistableRelayPinState : public switchPinState
{
  public:
    bistableRelayPinState(const char *init)
        : switchPinState(init)
    {
    }

    void switchState(uint8_t state) override
    {
        IO.pumpDigital(_action_pin, BISTABLE_RELAY_PUMP_DELAY);
    }

    static const char* ClassName() { return "BISTABLE_RELAY";}
};

#endif