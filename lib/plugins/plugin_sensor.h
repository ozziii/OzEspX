#ifndef PLUGIN_SENSOR_H
#define PLUGIN_SENSOR_H

class plugin_sensor
{
  public:
    plugin_sensor(){}

    virtual void execute_sensor() = 0;
    
    unsigned long sensor_delay;
};

#endif