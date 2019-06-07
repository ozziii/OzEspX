#ifndef WIFI_O_H
#define WIFI_O_H

#include <functional>
#include <vector>

#ifdef ESP32
#include <WiFi.h>
#include <FS.h>
#else
#include <ESP8266WiFi.h>
#include <Hash.h>
#endif

#include <configuration.h>
#include <debug_o.h>
#include <setting.h>
#include <utils.h>

#ifdef DEBUG
//#define WIFI_O_DEBUG
#endif

typedef struct
{
  String ssid;
  String pass;
  bool scanned;
  IPAddress ip;
  IPAddress gw;
  IPAddress netmask;
  IPAddress dns;
  int32_t rssi;
  uint8_t security;
  uint8_t channel;
  uint8_t bssid[6];
} network_t;

typedef enum
{
  STATE_STA_CONNECTED,
  STATE_STA_CONNECTION,
  STATE_STA_NOT_CONNECTED,
  STATE_STA_CONNECTING,
  STATE_STA_FAILED,
  STATE_AP_CREATE,
  STATE_AP_OK,
  STATE_AP_FAILED,
  STATE_SCAN_START,
  STATE_SCANIING,
  STATE_SCAN_COMPLETE,
  STATE_SCAN_NO_NETWORK,
  STATE_SCAN_FAILED,
} wifi_o_states_t;

#define WIFI_O_STA_CONNECTION_TRY 3
#define WIFI_O_STA_CONNECTING_TIMEOUT 30000
#define WIFI_O_SCANNING_TIMEOUT 30000
#define WIFI_O_AP_TIMEOUT 60000

class wifi_o
{
public:
  void Begin();
  bool loop();
  bool connected();

private:
  std::vector<network_t> _network_list;
  wifi_o_states_t _state;
  unsigned long _timeout;
  uint8_t _sta_connection_try;
  String espname;

  
  void _change_state(wifi_o_states_t state);

  void add_new_network(String SSID, String PASSWORD);
  bool _populate_network(uint8_t networkCount);

  void _startSTA();
  void _startSCAN();
  bool _startAP();
};
#endif