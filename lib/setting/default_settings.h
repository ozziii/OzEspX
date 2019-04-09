#ifndef DEFAULT_SETTINGS_h
#define DEFAULT_SETTINGS_h

#include <Arduino.h>

#define MAX_PLUGIN 12

struct Parameter
{
  uint8_t size;
  const char* value;
  const char* key;
};


const Parameter Parameters[] PROGMEM = {
    { 1   , "0" ,"init"},
    { 20  , "TEST" ,"espname" }, 
    { 20  , "admin" ,"WEBUSER" }, 
    { 20  , "admin" ,"WEBPASS" },
    { 20  , "ozziii.duckdns.org" ,"MQTTSIP" },
    { 20  , "11883" ,"MQTTPORT" },
    { 20  , "admin" ,"MQTTUSER" },
    { 20  , "admin" ,"MQTTPASS" },
    { 100 , "" ,"ddnsUrl" },
    { 20  , "lukegeorge" , "WIFI1SSID"},
    { 50  , "microelettronica"  , "WIFI1PASS"},
    { 20  , "Alice-62361025"  , "WIFI2SSID" },
    { 50  , "camillis1991de2000manuelamarialu"  , "WIFI2PASS"},
    { 20  , "" , "WIFI3SSID"},
    { 50  , "" , "WIFI3PAS"},
    { 16  , "" , "IPADDRESS"},   //IP ADDRESS
    { 16  , "192.168.0.1" , "GATEWAY" },     //GATEWAY
    { 16  , "255.255.255.0" , "SUBNETMASK" },   //SUBNET MASK
    { 16  , "8.8.8.8" , "DNS" },         //DNS
    { 5   , "80"  , "WEBPORT" },              //Server PORT
    { 5   , "500" ,"DebouncingInterrupt" }, 
    { 25   , "" ,"ITEM0" },
    { 25   , "" ,"ITEM1" },
    { 25   , "" ,"ITEM2" },
    { 25   , "" ,"ITEM3" },
    { 25   , "" ,"ITEM4" },
    { 25   , "" ,"ITEM5" },
    { 25   , "" ,"ITEM6" },
    { 25   , "" ,"ITEM7" },
    { 25   , "" ,"ITEM8" },
    { 25   , "" ,"ITEM9" },
    { 25   , "" ,"ITEM10" },
}; 

#endif