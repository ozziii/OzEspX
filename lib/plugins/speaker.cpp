#include "speaker.h"

#ifdef PLUGIN_SPEAKER
#include "freertos/ringbuf.h"

static volatile bool speaker_running = false;

WiFiUDP _speaker_udp_object;

static void speaker_task(void *arg)
{
    uint8_t *incomingPacket = new uint8_t[SPEAKER_PLUGIN_BUFFER_LENGTHT]; // buffer for incoming packetSize
    int16_t *sendPacket = new int16_t[SPEAKER_PLUGIN_BUFFER_LENGTHT];     // buffer for incoming packetSize

    size_t bytes_written;
    int len = 0;
    int packetSize = 0;

    _speaker_udp_object.flush();

    while (true)
    {
        packetSize = _speaker_udp_object.parsePacket();

        if (packetSize)
        {
            do
            {
                len = _speaker_udp_object.read(incomingPacket, SPEAKER_PLUGIN_BUFFER_LENGTHT);

                if (speaker_running)
                {
                    int16_t *tempPointer = (int16_t *)incomingPacket;

                    for (uint16_t i = 0; i < len / 2; i++)
                    {
                        sendPacket[(i * 2)] = sendPacket[(i * 2) + 1] = tempPointer[i];
                    }

                    i2s_write(SPEAKER_PLUGIN_I2S_PORT, sendPacket, len * 2, &bytes_written, (portTickType)portMAX_DELAY);
                }
                else
                {
                    delay(300);
                }

                packetSize = packetSize - len;

            } while (packetSize > 0);
        }
    }
}

speaker::speaker(const char *init)
{
    std::vector<String> config = splitString(init, PLUGIN_INIT_SEPARATOR_CHAR);

    if (config.size() >= 5)
    {
        this->name = config[1];
        this->_bck_pin = atoi(config[2].c_str());
        this->_rlc_pin = atoi(config[3].c_str());
        this->_din_pin = atoi(config[4].c_str());

        if (config.size() >= 6)
        {
            this->_is_enabled = true;
            //this->_en_pin = atoi(config[5].c_str());
            //IO.setMode(this->_en_pin, OUTPUT_MODE);
            //IO.writeDigital(this->_en_pin, LOW);
        }

        personal_topic_state = buildTopic(MQTT_COMMAND_STATE);

        personal_topic_action = buildTopic(MQTT_COMMAND_ACTION);
        Network.subscribe(personal_topic_action.c_str());

        personal_topic_volume = buildTopic(MQTT_COMMAND_VOLUME);
        Network.subscribe(personal_topic_volume.c_str());

        general_topic_action = SPEAKER_PLUGIN_GENERAL_TOPIC;
        Network.subscribe(general_topic_action.c_str());

        this->initialized = true;

#ifdef DEBUG_LOG
        DEBUG_MSG_P(PSTR("[SPEAKER][%s] IS INITIALZED BCK: %d  RLC: %d DIN: %d  \n"),
                    this->name.c_str(),
                    _bck_pin,
                    _rlc_pin,
                    _din_pin);

        this->_dac = new TAS5731(
            this->_reset_pin,
            this->_pdn_pin,
            this->_pvdd_pin,
            true,
            this->_sda_pin,
            this->_scl_pin
        );

    }
    else
    {

        DEBUG_MSG_P(PSTR("[SPEAKER][ERROR] WRONG INITIALZE STRING \n"));
#endif
    }
}

bool speaker::sendResponse(String topic, String Message)
{
    if (topic.equals(this->general_topic_action))
    {
        if (Message.equals(MQTT_STATE_OFF))
        {
            this->_state_general = false;

            this->stop_speaker();

            if (this->_state_server)
            {
                this->stop_udp();
            }

            return true;
        }

        if (Message.equals(MQTT_STATE_ON))
        {
            this->_state_general = true;

            if (this->_state_internal)
            {
                this->start_udp();
                this->start_speaker();
            }

            return true;
        }
    }

    if (topic.equals(this->personal_topic_volume))
    {
        double volume = atoi(Message.c_str());

        if (SPEAKER_PLUGIN_LOW_GAIN <= volume && volume <= SPEAKER_PLUGIN_TOP_GAIN)
        {
            this->_dac->setVolume(volume);
        }

        return true;
    }

    if (topic.equals(this->personal_topic_action))
    {
        if (Message.equals(MQTT_STATE_OFF))
        {
            this->_state_internal = false;
            Network.send(this->personal_topic_state.c_str(), MQTT_STATE_OFF);

            this->stop_speaker();

            return true;
        }

        if (Message.equals(MQTT_STATE_ON))
        {
            this->_state_internal = true;
            Network.send(this->personal_topic_state.c_str(), MQTT_STATE_ON);

            if (this->_state_general)
            {
                if (!this->_state_server)
                {
                    this->start_udp();
                }

                this->start_speaker();
            }

            return true;
        }
    }
    return false;
}

void speaker::start_udp()
{
    if (!this->_i2s_is_init)
    {
        this->i2s_init();

        this->multiacst_address.fromString(SPEAKER_PLUGIN_MULTICAST_IP);
        _speaker_udp_object.beginMulticast(this->multiacst_address, SPEAKER_PLUGIN_MULTICAST_PORT);
        xTaskCreate(speaker_task, "i2c_speak_tk", SPEAKER_PLUGIN_BUFFER_LENGTHT * 8, NULL, 1, &this->task_handle);

        this->_i2s_is_init = true;
    }

    speaker_running = true;
    this->_state_server = true;
}

void speaker::stop_udp()
{
    speaker_running = false;
    this->_state_server = false;
}

void speaker::start_speaker()
{
    if (_is_enabled)
        this->_dac->Initialization();
        this->_dac->set_registers(Default, tas_default_count);
        this->_dac->set_bit_per_sample(16);
        this->_dac->Exit_Shutdown();

        delay(500);
        this->_dac->setVolume(SPEAKER_PLUGIN_DEFAULT_GAIN);
        delay(100);
}

void speaker::stop_speaker()
{
    if (this->_is_enabled)
        this->_dac->Enter_Shutdown();
        this->_dac->Power_Down();
}

void speaker::i2s_init()
{
    i2s_config_t i2s_config_rx;

    i2s_config_rx.mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX);
    i2s_config_rx.sample_rate = SPEAKER_PLUGIN_SAMPLE_RATE;
    i2s_config_rx.bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT;
    i2s_config_rx.channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT;
    i2s_config_rx.communication_format = I2S_COMM_FORMAT_I2S_MSB;
    i2s_config_rx.dma_buf_count = 8;                       // number of buffers, 128 max.
    i2s_config_rx.dma_buf_len = 32 * 2;                    // size of each buffer
    i2s_config_rx.intr_alloc_flags = ESP_INTR_FLAG_LEVEL1; // Interrupt level 1
    i2s_config_rx.tx_desc_auto_clear = true;
    i2s_config_rx.use_apll = false;

    i2s_pin_config_t pin_config_tx = {
        .bck_io_num = this->_bck_pin,
        .ws_io_num = this->_rlc_pin,
        .data_out_num = this->_din_pin,
        .data_in_num = I2S_PIN_NO_CHANGE};

    i2s_driver_install(SPEAKER_PLUGIN_I2S_PORT, &i2s_config_rx, 0, NULL);
    i2s_set_pin(SPEAKER_PLUGIN_I2S_PORT, &pin_config_tx);

    REG_WRITE(PIN_CTRL, 0xFF0);
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO0_U, FUNC_GPIO0_CLK_OUT1);

    //this->_i2s_is_init = true;
}

#endif