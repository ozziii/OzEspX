#include <LoraToMqtt.h>

LoraToMqtt::LoraToMqtt(const char *init)
{
    std::vector<String> config = splitString(init, PLUGIN_INIT_SEPARATOR_CHAR);

    if (config.size() >= 8)
    {
        this->name = config[1];
        this->_address = atoi(config[2].c_str());
        this->_channel = atoi(config[3].c_str());
        this->_aux = atoi(config[4].c_str());
        this->_m0 = atoi(config[5].c_str());
        this->_m1 = atoi(config[6].c_str());
        this->_uart_number = atoi(config[7].c_str());


        this->_root_topic = buildTopic(String());
        Network.subscribe(this->_root_topic.c_str());

        this->sensor_delay = LORA_UART_DELAY;
        HardwareSerial serial = HardwareSerial(this->_uart_number);
        this->_lora = new LoRa_E32(&serial, this->_aux, this->_m1, this->_m0);
        this->_protocoll = new e32cp(_lora, this->_address, this->_channel);

        if (this->_protocoll->begin())
        {
            if (this->_protocoll->config())
            {

            }
        }
    }
}

bool LoraToMqtt::sendResponse(String topic, String Message)
{
    std::vector<String> topic_array = splitString(topic.c_str(), PLUGIN_INIT_SEPARATOR_CHAR);

    if(topic_array.size() != 4 ) return false;

    if(!topic_array[0].equals(SETTING_P(PSTR("espname"))) ) return false;

    if(!topic_array[1].equals(this->name)) return false;

    uint8_t address = atoi(topic_array[2].c_str());
    
    String loramessage = topic_array[3] + PLUGIN_INIT_SEPARATOR_CHAR + Message;
 
    this->_protocoll->sleepyWake(address,this->_channel,loramessage);

    return true;
}

void LoraToMqtt::execute_sensor()
{
    if (this->_protocoll->available())
    {
        String lora_message = this->_protocoll->ServerRecieve();

        if(lora_message.length() == 0) return;

        std::vector<String> message_array = splitString(lora_message.c_str(), PLUGIN_INIT_SEPARATOR_CHAR);

        if(message_array.size() != 3) return;


        String address = message_array[0];
        String status = message_array[1];
        String message = message_array[2];

        String topic = this->_root_topic + PLUGIN_INIT_SEPARATOR_CHAR + address + PLUGIN_INIT_SEPARATOR_CHAR + status;


        Network.send(topic.c_str(),message.c_str());

    }
}