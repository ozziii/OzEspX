#include <plugins.h>

#ifdef ESP32
const html_plugin plugin_list[] PROGMEM = {
#else
const html_plugin plugin_list[] = {
#endif

    {FreeRAMPlugin::ClassName(), "[*]/[NAME]/[delay]"},

    {ddns::ClassName(), "[*]/[NAME]/[delay]"},

    {DHT22Plugin::ClassName(), "[*]/[NAME]/[delay]/[pin]"},

    {switchPinState::ClassName(), "[*]/[NAME]/[out pin]/[read pin]/[delay]=0/[out logic 1:0]=1/[read logic 1:0]=1/[start out pin 1:0]=0/[Pull up 1:0]=0"},

    {bistableRelayPinState::ClassName(), "[*]/[NAME]/[out pin]/[read pin]/[delay]=0/[out logic 1:0]=1/[read logic 1:0]=1/[start out pin 1:0]=0/[Pull up 1:0]=0"},

    {switchSelfState::ClassName(), "[*]/[NAME]/[pin]/[delay]=0/[logic 1:0]=1/[start 1:0]=0"},

    {led_plugin::ClassName(), "[*]/[NAME]/[min 0:1024]/[max 0:1024]/[Wite pin or R-G-B ]/[Dimmer Logic 1:0] = 1/[On-Off pin]?/[ON-Off Logic 1:0] = 1"},

    {bistableRelayPinStateNI::ClassName(), "[*]/[NAME]/[action pin]/[read pin]/[delay]=0/[action logic 1:0]=1/[read logic 1:0]=1/[start action pin 1:0]=0/[Pull up 1:0]=0"},

    {binary_sensor::ClassName(), "[*]/[NAME]/[read pin]/[delay (s)]=0/[read logic 1:0]=1/[Pull up 1:0]=0"},

    {BME280Plugin::ClassName(), "[*]/[NAME]/[delay]" },

    {bell_plugin::ClassName(), "[*]/[NAME]/[action pin]/[action logic 1:0]=1/[start pin 1:0]=0" },

#ifdef PLUGIN_PZEM004T
    {PZEM004T_o::ClassName(), "[*]/[NAME]/[delay]/[Serial or (Tx_pin-Rx_pin)]"},
#endif

#ifdef PLUGIN_JSNSR04TV2
    {JSNSR04TV2::ClassName(), "[*]/[NAME]/[delay]/[trig pin]/[echo pin]"},
#endif

#ifdef PLUGIN_RADIOFREQUENCY_REVICER
    {bistableRelayPinStateNI::ClassName(), "[*]/[NAME]/[sensor pin]/[switch pin]/[relay logic 1:0]/[valid key]"},
#endif

#ifdef NEXTION
    {nextion_plugin::ClassName(), "[*]/[NAME]/[delay (ms)]/[Hardware Serial]"},
#endif


#ifdef PLUGIN_SPEAKER
    {speaker::ClassName(), "[*]/[NAME]/[BCK]/[RLC]/[DIN]/[ENABLE PIN ?]"},
#endif

#ifdef PLUGIN_I2S2UDP
    {i2s2udp::ClassName(), "[*]/[NAME]/[BCK]/[RLC]/[DIN]"},
#endif

#ifdef PLUGIN_VMC
    {vmc_plugin::ClassName(), "[*]/[NAME]/[ENABLE]/[PWM]/[DIRECTION]"},
#endif

#ifdef PLUGIN_LORATOMQTT
    {LoraToMqtt::ClassName(), "[*]/[NAME]/[AUX]/[M0]/[M1]/[UART_NUMBER]"},
#endif

};

void plugins_o::createPlugIn(
    String initString,
    std::vector<plugin_sensor *> *_sensors,
    std::vector<plugin_interrupt *> *_interrups,
    std::vector<plugin_response *> *_responses)
{
    // empty string check
    if (initString.length() == 0)
        return;

    int index = initString.indexOf('/');

    // no rigth formatted string
    if (index < 0)
        return;

    uint8_t key = atoi(initString.substring(0, index).c_str());

    if (key >= plugin_list_size())
        return;

    const char *name = Get_CONFIG_P(&plugin_list[key]).name;

    //************** FREE RAM PLUGIN *************
    if (strcmp(name, FreeRAMPlugin::ClassName()) == 0)
    {
        FreeRAMPlugin *item = new FreeRAMPlugin(initString.c_str());
        if (item->isInitialized())
        {
            _sensors->push_back(item);
        }
        return;
    }

    //************** DDNS PLUGIN *************
    if (strcmp(name, ddns::ClassName()) == 0)
    {
        ddns *item = new ddns(initString.c_str());
        if (item->isInitialized())
        {
            _sensors->push_back(item);
        }
        return;
    }

    //************** DHT22 PLUGIN *************
    if (strcmp(name, DHT22Plugin::ClassName()) == 0)
    {
        DHT22Plugin *item = new DHT22Plugin(initString.c_str());
        if (item->isInitialized())
        {
            _sensors->push_back(item);
        }
        return;
    }

    //************** SWITCH PIN STATE PLUGIN *************
    if (strcmp(name, switchPinState::ClassName()) == 0)
    {
        switchPinState *item = new switchPinState(initString.c_str());
        if (item->isInitialized())
        {
            _interrups->push_back(item);
            _responses->push_back(item);
            _sensors->push_back(item);
        }
        return;
    }

    //************** BISTABLE RELAY PLUGIN *************
    if (strcmp(name, bistableRelayPinState::ClassName()) == 0)
    {
        bistableRelayPinState *item = new bistableRelayPinState(initString.c_str());
        if (item->isInitialized())
        {
            _interrups->push_back(item);
            _responses->push_back(item);
            _sensors->push_back(item);
        }
        return;
    }

    //************** SWITCH PIN STATE PLUGIN *************
    if (strcmp(name, switchSelfState::ClassName()) == 0)
    {
        switchSelfState *item = new switchSelfState(initString.c_str());
        if (item->isInitialized())
        {
            _responses->push_back(item);
            _sensors->push_back(item);
        }
        return;
    }


    //************** BME280 *************
    if (strcmp(name, BME280Plugin::ClassName()) == 0)
    {
        BME280Plugin *item = new BME280Plugin(initString.c_str());
        if (item->isInitialized())
        {
            _sensors->push_back(item);
        }
        return;
    }


    //************** BELL *************
    if (strcmp(name, bell_plugin::ClassName()) == 0)
    {
        bell_plugin *item = new bell_plugin(initString.c_str());
        if (item->isInitialized())
        {
            _responses->push_back(item);
        }
        return;
    }

#ifdef PLUGIN_JSNSR04TV2
    //************** JSN-SR04T-V2 PLUGIN *************
    if (strcmp(name, JSNSR04TV2::ClassName()) == 0)
    {
        JSNSR04TV2 *item = new JSNSR04TV2(initString.c_str());
        if (item->isInitialized())
        {
            _sensors->push_back(item);
        }
        return;
    }
#endif

#ifdef PLUGIN_PZEM004T
    //************** PZEM_004T PLUGIN *************
    if (strcmp(name, PZEM004T_o::ClassName()) == 0)
    {
        PZEM004T_o *item = new PZEM004T_o(initString.c_str());
        if (item->isInitialized())
        {
            _sensors->push_back(item);
        }
        return;
    }
#endif

    //************** LED DIMMER PLUGIN *************
    if (strcmp(name, led_plugin::ClassName()) == 0)
    {
        led_plugin *item = new led_plugin(initString.c_str());
        if (item->isInitialized())
        {
            _responses->push_back(item);
        }
        return;
    }

    //************** BISTABLE RELAY PLUGIN *************
    if (strcmp(name, bistableRelayPinStateNI::ClassName()) == 0)
    {
        bistableRelayPinStateNI *item = new bistableRelayPinStateNI(initString.c_str());
        if (item->isInitialized())
        {
            _responses->push_back(item);
            _sensors->push_back(item);
        }
        return;
    }

    //************** BINARY SENSOR *************
    if (strcmp(name, binary_sensor::ClassName()) == 0)
    {
        binary_sensor *item = new binary_sensor(initString.c_str());
        if (item->isInitialized())
        {
            _interrups->push_back(item);
            _sensors->push_back(item);
        }
        return;
    }

#ifdef PLUGIN_RADIOFREQUENCY_REVICER

    //************** Radio Frequenci Switch PLUGIN *************
    if (strcmp(name, rf_rx_switch::ClassName()) == 0)
    {
        rf_rx_switch *item = new rf_rx_switch(initString.c_str());
        if (item->isInitialized())
        {
            _responses->push_back(item);
        }
        return;
    }
#endif

#ifdef NEXTION
    //************** NEXTION DISPLAY PUGIN *************
    if (strcmp(name, nextion_plugin::ClassName()) == 0)
    {
        nextion_plugin *item = new nextion_plugin(initString.c_str());
        if (item->isInitialized())
        {
            _responses->push_back(item);
            _sensors->push_back(item);
        }
        return;
    }
#endif


#ifdef PLUGIN_SPEAKER
    //************** SPEAKER i2s PUGIN *************
    if (strcmp(name, speaker::ClassName()) == 0)
    {
        speaker *item = new speaker(initString.c_str());
        if (item->isInitialized())
        {
            _responses->push_back(item);
        }
        return;
    }
#endif


#ifdef PLUGIN_I2S2UDP
    //**************  i2s to UDP PUGIN *************
    if (strcmp(name, i2s2udp::ClassName()) == 0)
    {
        i2s2udp *item = new i2s2udp(initString.c_str());
        if (item->isInitialized())
        {
            _sensors->push_back(item);
        }
        return;
    }
#endif

#ifdef PLUGIN_VMC
    //**************  VMC PUGIN *************
    if (strcmp(name, vmc_plugin::ClassName()) == 0)
    {
        vmc_plugin *item = new vmc_plugin(initString.c_str());
        if (item->isInitialized())
        {
            _responses->push_back(item);
        }
        return;
    }
#endif

#ifdef PLUGIN_LORATOMQTT
    //**************  VMC PUGIN *************
    if (strcmp(name, LoraToMqtt::ClassName()) == 0)
    {
        LoraToMqtt *item = new LoraToMqtt(initString.c_str());
        if (item->isInitialized())
        {
            _responses->push_back(item);
            _sensors->push_back(item);
        }
        return;
    }
#endif

};

html_plugin plugins_o::get(uint8_t index)
{
#ifdef ESP32
    return Get_CONFIG_P(&plugin_list[index]);
#else
    return plugin_list[index];
#endif
}

uint8_t plugins_o::plugin_list_size()
{
    return Size_P(plugin_list);
}
