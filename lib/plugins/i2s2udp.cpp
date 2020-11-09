#include "i2s2udp.h"

#ifdef PLUGIN_I2S2UDP

// ###### UART COMUNICATION PROTOCOL
#define UART_CONNECTED 0x01
#define UART_DISCONNECTED 0x02
#define UART_START_PLAY 0x03
#define UART_STOP_PLAY 0x04

#define UART_SAMPLE_RATE_1600 0x91
#define UART_SAMPLE_RATE_32000 0x92
#define UART_SAMPLE_RATE_44100 0x93
#define UART_SAMPLE_RATE_48000 0x94
// #END# UART COMUNICATION PROTOCOL

static volatile bool i2stoUdp_running = false;

// ###### UDP OBJECT FOR SENDING
WiFiUDP _i2s_udp_object;

/****************************************************************
*            
*     i2s  Trasmission Task
*
***************************************************************/
static void i2s_tx_task(void *arg)
{
    size_t bytes_read = 0;
    uint8_t buf_ptr_read[I2S2UDP_BUFFER_LENGTH];
    uint8_t buf_ptr_send[I2S2UDP_BUFFER_LENGTH / 2];

    uint16_t r_temp = 0;
    uint16_t l_temp = 0;

    while (true)
    {
        i2s_read(I2S2UDP_PLUGIN_I2S_PORT, buf_ptr_read, I2S2UDP_BUFFER_LENGTH, &bytes_read, portMAX_DELAY);

        if (bytes_read > 0)
        {
            for (int i = 0; i < bytes_read / 4; i++)
            {
                /*
                r_temp = ((uint16_t)buf_ptr_read[(i * 4) + 1] << 8) | buf_ptr_read[(i * 4) + 0];
                l_temp = ((uint16_t)buf_ptr_read[(i * 4) + 3] << 8) | buf_ptr_read[(i * 4) + 2];
                r_temp = (r_temp + l_temp) / 2;
                buf_ptr_send[i * 2] = r_temp & 0xff;
                buf_ptr_send[(i * 2) + 1] = r_temp >> 8;
                */
                buf_ptr_send[i * 2] = buf_ptr_read[(i * 4) + 0];
                buf_ptr_send[(i * 2) + 1] = buf_ptr_read[(i * 4) + 1];
            }

            if (i2stoUdp_running)
            {
                _i2s_udp_object.beginMulticastPacket();
                _i2s_udp_object.write(buf_ptr_send, bytes_read / 2);
                _i2s_udp_object.endPacket();
            }
        }
        else
        {
            //
            vTaskDelay(200 / portTICK_PERIOD_MS);
        }
    }
}

/********************************************
 * 
 *     CTOR
 * 
 * **************************************************/
i2s2udp::i2s2udp(const char *init)
{

    if (!this->multiacst_address.fromString(SPEAKER_PLUGIN_MULTICAST_IP))
    {
        OZ_LOG_E_P(PSTR("[i2s2Udp][ERROR] WRONG IP ADDRESS \n"));
        return;
    }

    std::vector<String> config = splitString(init, PLUGIN_INIT_SEPARATOR_CHAR);

    if (config.size() >= 5)
    {
        this->name = config[1];
        this->_bck_pin = atoi(config[2].c_str());
        this->_rlc_pin = atoi(config[3].c_str());
        this->_din_pin = atoi(config[4].c_str());

        this->sensor_delay = I2S2UDP_SERIAL_DELAY;

        this->initialized = true;

        OZ_LOG_I_P(PSTR("[i2s2Udp][%s] IS INITIALZED BCK: %d  RLC: %d DIN: %d  \n"),
                   this->name.c_str(),
                   _bck_pin,
                   _rlc_pin,
                   _din_pin);
    }
    else
    {
        OZ_LOG_E_P(PSTR("[i2s2Udp][%s][ERROR] WRONG INITIALZE STRING \n"), this->name.c_str());
    }
}

void i2s2udp::execute_sensor()
{
    if (!this->_serial_init)
    {
        I2S2UDP_SERIAL.begin(I2S2UDP_SERIAL_BAUD_RATE);
        this->_serial_init = true;
    }

    if (I2S2UDP_SERIAL.available() < 1)
        return;

    int incomingByte = I2S2UDP_SERIAL.read();

    if (incomingByte < 1)
        return;

    switch (incomingByte)
    {
    case UART_CONNECTED:
        break;
    case UART_DISCONNECTED:
        break;

    case UART_START_PLAY:
        //this->init_i2s();

        //if (!this->_udp_init)
        //{
        //    _i2s_udp_object.beginMulticast(multiacst_address, SPEAKER_PLUGIN_MULTICAST_PORT);
        //    this->_udp_init = true;
        //}

        //xTaskCreate(i2s_tx_task, "I2Stx", I2S2UDP_BUFFER_LENGTH * 8, NULL, 1, &this->i2s_tx_task_handle);

        if (!this->_udp_init)
        {
            this->init_i2s();
            _i2s_udp_object.beginMulticast(multiacst_address, SPEAKER_PLUGIN_MULTICAST_PORT);
            xTaskCreate(i2s_tx_task, "I2Stx", I2S2UDP_BUFFER_LENGTH * 8, NULL, 1, &this->i2s_tx_task_handle);
            this->_udp_init = true;
        }

        i2stoUdp_running = true;

        Network.send(SPEAKER_PLUGIN_GENERAL_TOPIC, MQTT_STATE_ON);

        break;

    case UART_STOP_PLAY:
        Network.send(SPEAKER_PLUGIN_GENERAL_TOPIC, MQTT_STATE_OFF);
        i2stoUdp_running = false;
        //if (this->i2s_tx_task_handle)
        //{
        //    vTaskDelete(this->i2s_tx_task_handle);
        //    this->i2s_tx_task_handle = NULL;
        //}

        //this->stop_i2s();
        break;

    case UART_SAMPLE_RATE_1600:
        /* code */
        break;
    case UART_SAMPLE_RATE_32000:
        /* code */
        break;
    case UART_SAMPLE_RATE_44100:
        /* code */
        break;
    case UART_SAMPLE_RATE_48000:
        /* code */
        break;

    default:
        break;
    }
}

void i2s2udp::init_i2s()
{
    if (this->_i2s_init)
        return;

    i2s_config_t i2s_config_rx;

    i2s_config_rx.mode = (i2s_mode_t)(I2S_MODE_SLAVE | I2S_MODE_RX);
    i2s_config_rx.sample_rate = SPEAKER_PLUGIN_SAMPLE_RATE;
    i2s_config_rx.bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT;
    i2s_config_rx.channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT;
    i2s_config_rx.communication_format = I2S_COMM_FORMAT_I2S; // !!! IMPORTANT!!!!
    i2s_config_rx.dma_buf_count = 32;                         // number of buffers, 128 max.
    i2s_config_rx.dma_buf_len = 32 * 2;                       // size of each buffer
    i2s_config_rx.intr_alloc_flags = ESP_INTR_FLAG_LEVEL1;    // Interrupt level 1
    i2s_config_rx.use_apll = true;

    i2s_driver_install(I2S2UDP_PLUGIN_I2S_PORT, &i2s_config_rx, 0, NULL);

    i2s_pin_config_t pin_config_rx;
    pin_config_rx.bck_io_num = this->_bck_pin;
    pin_config_rx.data_in_num = this->_din_pin;
    pin_config_rx.ws_io_num = this->_rlc_pin;
    pin_config_rx.data_out_num = I2S_PIN_NO_CHANGE;

    i2s_set_pin(I2S2UDP_PLUGIN_I2S_PORT, &pin_config_rx);

    OZ_LOG_V_P(PSTR("[i2s2Udp][INFO] I2S INITAILIZED \n"));

    this->_i2s_init = true;
}

void i2s2udp::stop_i2s()
{
    if (!this->_i2s_init)
        return;

    i2s_stop(I2S2UDP_PLUGIN_I2S_PORT);
    i2s_driver_uninstall(I2S2UDP_PLUGIN_I2S_PORT);

    OZ_LOG_V_P(PSTR("[i2s2Udp][INFO] I2S UNISTALL \n"));

    this->_i2s_init = false;
}

#endif