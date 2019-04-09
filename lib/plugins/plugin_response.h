#ifndef PLUGIN_SWITCH_H
#define PLUGIN_SWITCH_H

#include <Arduino.h>

class plugin_response
{
    public:
        plugin_response(){}

        virtual bool sendResponse(String topic, String message) = 0;
};

#endif