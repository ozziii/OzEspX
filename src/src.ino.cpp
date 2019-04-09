# 1 "c:\\users\\ozzii\\appdata\\local\\temp\\tmphyxeos"
#include <Arduino.h>
# 1 "C:/Users/ozzii/Dropbox/Develop/Arduino/OzEsp/src/src.ino"
#include <Arduino.h>



#include <configuration.h>

#include <system_o.h>

#include <network.h>

#include <webserver.h>

#include <executor.h>



static system_o System;

static executor Program;



#ifdef ESP32

ticker_o web_tiker;
void web_callback();
void callback(char *p_topic, char *p_payload, size_t p_length);
void setup();
void loop();
#line 27 "C:/Users/ozzii/Dropbox/Develop/Arduino/OzEsp/src/src.ino"
void web_callback()

{

    WebServer.begin(callback);

}

#endif



void callback(char *p_topic, char *p_payload, size_t p_length)

{

    char message[p_length + 1];

    strlcpy(message, (char *)p_payload, p_length + 1);

    Program.mqttrecive(p_topic, message);

}



void setup()

{

#ifdef DEBUG

    debug::debugBegin();

#endif





    Setting.begin();



#ifdef INIT_SETTING

    Setting.RestoreDefault();

#endif



    System.begin();



    if (System.checkSystemStability())

    {



        Program.begin();

    }





    Network.begin(callback);







#ifdef ESP32

    web_tiker.once(WEBSERVER_START_DELAY, web_callback);

#else

    WebServer.begin(callback);

#endif





}



void loop()

{

    Network.loop();

    Program.loop();



    System.loop();

}