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
    { 20  , "NoName" ,"espname" }, 
    { 20  , "admin" ,"WEBUSER" }, 
    { 20  , "admin" ,"WEBPASS" },
    { 20  , "" ,"MQTTSIP" },
    { 20  , "" ,"MQTTPORT" },
    { 20  , "admin" ,"MQTTUSER" },
    { 20  , "admin" ,"MQTTPASS" },
    { 100 , "" ,"ddnsUrl" },
    { 20  , "" , "WIFI1SSID"},
    { 50  , ""  , "WIFI1PASS"},
    { 20  , ""  , "WIFI2SSID" },
    { 50  , ""  , "WIFI2PASS"},
    { 20  , "" , "WIFI3SSID"},
    { 50  , "" , "WIFI3PAS"},
    { 16  , "" , "IPADDRESS"},   //IP ADDRESS
    { 16  , "" , "GATEWAY" },     //GATEWAY
    { 16  , "" , "SUBNETMASK" },   //SUBNET MASK
    { 16  , "" , "DNS" },         //DNS
    { 5   , "80"  , "WEBPORT" },              //Server PORT
    { 5   , "200" ,"DebouncingInterrupt" }, 
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