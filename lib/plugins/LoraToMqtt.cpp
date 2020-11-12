#include <LoraToMqtt.h>

LoraToMqtt::LoraToMqtt(const char *init)
{
    std::vector<String> config = splitString(init, PLUGIN_INIT_SEPARATOR_CHAR);

    if (config.size() == 6)
    {
         
        //AUX/M0/M1/UART_NUMBER

        this->name = config[1];
        this->_aux = atoi(config[2].c_str());
        this->_m0 = atoi(config[3].c_str());
        this->_m1 = atoi(config[4].c_str());
        this->_uart_number = atoi(config[5].c_str());

        this->_root_topic = buildTopic(String('#'));

        Network.subscribe(this->_root_topic.c_str());

        OZ_LOG_V_P(PSTR("[LORA][%s] SUBSCRIBE MULTILEVEL TOPIC %s \n "), this->name.c_str(), this->_root_topic.c_str());

        this->sensor_delay = LORA_UART_DELAY;
        this->_serial = new HardwareSerial(this->_uart_number);
        this->_lora = new LoRa_E32(this->_serial, this->_aux, this->_m0, this->_m1);
        this->_protocoll = new e32cp(this->_lora);

        if (this->_protocoll->begin())
        {
            if (this->_protocoll->config())
            {
                OZ_LOG_I_P(PSTR("[LORA][%s] LORA TO MQTT CONFIG COMLETE \n "), this->name.c_str());
                OZ_LOG_I_P(PSTR("[LORA][%s] AUX:[%u] M0:[%u] M1:[%u] UART:[%u] \n "), 
                this->name.c_str(),
                this->_aux,
                this->_m0,
                this->_m1,
                this->_uart_number);

                this->initialized = true;
            }
            else
            {
                OZ_LOG_W_P(PSTR("[LORA][%s][ERROR] CONFIG ERROR \n "), this->name.c_str());
            }
        }
        else
        {
            OZ_LOG_W_P(PSTR("[LORA][%s][ERROR] BEGIN ERROR \n "), this->name.c_str());
        }
    }
    else
    {
        OZ_LOG_W_P(PSTR("[LORA][%s][ERROR] WRONG INITIALZE STRING \n "), this->name.c_str());
    }
}

bool LoraToMqtt::sendResponse(String topic, String Message)
{
    std::vector<String> topic_array = splitString(topic.c_str(), PLUGIN_INIT_SEPARATOR_CHAR);

    if (topic_array.size() != 4)
        return false;

    if (!topic_array[0].equals(SETTING_P(PSTR("espname"))))
        return false;

    if (!topic_array[1].equals(this->name))
        return false;

    uint8_t client_address = atoi(topic_array[2].c_str());

    String loramessage = topic_array[3] + char(PLUGIN_INIT_SEPARATOR_CHAR) + Message;

    if (this->_protocoll->sleepyWake(client_address, E32_SERVER_CHANNEL, loramessage))
    {
        OZ_LOG_V_P(PSTR("[LORA][%s] MQTT -> LORA | SEND WAKE COMMAND : [%s] TO CLIENT : [%u] \n "), this->name.c_str(), loramessage.c_str(), client_address);
    }
    else
    {
        OZ_LOG_E_P(PSTR("[LORA][%s][ERROR] PROBLEM WHEN SENDING LoRa \n "), this->name.c_str());
        OZ_LOG_E_P(PSTR("[LORA][%s][ERROR] Address : %u , Channel: %u \n "), this->name.c_str(), client_address, E32_SERVER_CHANNEL);
    }

    return true;
}

void LoraToMqtt::execute_sensor()
{
    if (this->_protocoll->available())
    {
        String lora_message = this->_protocoll->ServerRecieve();

        if (lora_message.length() == 0)
            return;

        std::vector<String> message_array = splitString(lora_message.c_str(), PLUGIN_INIT_SEPARATOR_CHAR);

        if (message_array.size() != 3)
            return;

        String client_address = message_array[0];
        String status = message_array[1];
        String message = message_array[2];

        String topic = this->buildTopic(String()) + char(PLUGIN_INIT_SEPARATOR_CHAR) + client_address + char(PLUGIN_INIT_SEPARATOR_CHAR) + status;

        Network.send(topic.c_str(), message.c_str());

        OZ_LOG_V_P(PSTR("[LORA][%s] LORA -> MQTT topic: [%s] message: [%S] \n "), this->name.c_str(), topic.c_str(), message.c_str());
    }
}