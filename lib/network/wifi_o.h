#ifndef WIFI_O_H
#define WIFI_O_H

#include <configuration.h>
#include <debug_o.h>
#include <setting.h>
#include <justWifi.h>
#include <utils.h>

class wifi_o
{
public:
  void Begin();
  bool loop();
  bool connected();

  private:
    JustWifi js;
    unsigned long lastcheck;
};
#endif