#ifndef DEFAULT_SETTINGS_h
#define DEFAULT_SETTINGS_h

#include <Arduino.h>


/*===========================================================================
* ============================= DATABASE CONFIGURATION ========================
* ===========================================================================*/

#define EEPROM_BEGIN_INDEX 5
#define STABILITY_INDEX 0
#define START_COUNT_INDEX 1
#define DB_VERSION_INDEX 2




#define DB_VERSION 5

#define MAX_PLUGIN 10

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
    { 20  , "1883" ,"MQTTPORT" },
    { 20  , "mqtt" ,"MQTTUSER" },
    { 20  , "hassio" ,"MQTTPASS" },
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
    { 30   , "" ,"ITEM0" },
    { 30   , "" ,"ITEM1" },
    { 30   , "" ,"ITEM2" },
    { 30   , "" ,"ITEM3" },
    { 30   , "" ,"ITEM4" },
    { 30   , "" ,"ITEM5" },
    { 30   , "" ,"ITEM6" },
    { 30  , "" ,"ITEM7" },
    { 30   , "" ,"ITEM8" },
    { 30   , "" ,"ITEM9" },
    { 30   , "" ,"ITEM10" },
}; 

#endif
