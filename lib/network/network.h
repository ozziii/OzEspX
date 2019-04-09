#ifndef NETWORK_H
#define NETWORK_H

#include <Arduino.h>

#include <wifi_o.h>
#include <mqtt_o.h>

class network
{
  public:
    network();  
    void begin( OnMessageMqttCallback callback);
    void loop();

    void subscribe(const char *topic);
    bool send(const char *topic, const char *message);

    bool wifiIsConnect();
    String getIpAddress();

    wifi_o wifi;
};

extern network Network;

#endif