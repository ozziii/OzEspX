#include "TAS5731.h"

TAS5731::TAS5731(uint8_t RESET, uint8_t PDN, uint8_t PVDD, bool PVDD_LOGIC, int SDA, int SCL)
{
    if (SDA == -1 || SCL == -1)
    {
        Wire.begin();
    }
    else
    {
        Wire.begin(SDA,SCL);
    }

    this->_reset_pin = RESET;
    pinMode(this->_reset_pin, OUTPUT);
    digitalWrite(this->_reset_pin, LOW);

    this->_pdn_pin = PDN;
    pinMode(this->_pdn_pin, OUTPUT);
    digitalWrite(this->_pdn_pin, LOW);

    this->_pvdd_pin = PVDD;
    this->_pvdd_logic = PVDD_LOGIC;
    pinMode(this->_pvdd_pin, OUTPUT);
    digitalWrite(this->_pvdd_pin, LOW ^ this->_pvdd_logic);
}

TAS5731::TAS5731(uint8_t RESET, uint8_t PDN, uint8_t PVDD, bool PVDD_LOGIC)
    : TAS5731(RESET,PDN,PVDD,PVDD_LOGIC, -1, -1)
{
}

void TAS5731::Initialization()
{
    /// 1)  PDN =  from 0  1 
    digitalWrite(this->_pdn_pin, HIGH);
    /// 2)  delay(100) 
    delay(100);
    //  3)  PVDD  to ON 
    digitalWrite(this->_pvdd_pin, HIGH ^ this->_pvdd_logic);

    //  4)  RESET to 1
    digitalWrite(this->_reset_pin, HIGH);
    //  5)  SET TRIM  (write 0x00 to register 0x1B)
    delay(20);

    uint8_t data[1] = {0x00};
    write_register(DDX_OSC_TRIM, data);
    //  6)  wait at least 50 ms.
    delay(50);
}

void TAS5731::Exit_Shutdown() 
{
    /// 1)  Write 0x00 to register 0x05. 
    uint8_t data[1] = {0x00};
    write_register(DDX_SYS_CTL_2, data);
    /// 2)  Wait at least 1 ms + 1.3 × tstart (where tstart is specified by register 0x1A)
    delay(50);
}

void TAS5731::Enter_Shutdown()
{
    /// 1)  Write 0x40 to register 0x05. 
    uint8_t data[1] = {0x40};
    write_register(DDX_SYS_CTL_2, data);
    /// 2)  Wait at least 1 ms + 1.3 × tstart (where tstart is specified by register 0x1A)
    delay(50);
}

void TAS5731::Power_Down()
{
    /// 1)  RESET = 0  
    digitalWrite(this->_reset_pin, LOW);
    /// 2)  wait at least 2 us.
    delay(100);
    //  3)  PVDD  to OFF
    digitalWrite(this->_pvdd_pin, LOW ^ this->_pvdd_logic);
    //  4)  PDN = 0
    digitalWrite(this->_pdn_pin, LOW);
}

bool TAS5731::setVolume(uint8_t volume)
{
    uint8_t valume_t = 255 - volume;
    uint8_t data[1] = {valume_t};
    return this->write_register(DDX_MASTER_VOLUME, data);
}

bool TAS5731::set_bit_per_sample(uint8_t bitXsample)
{
    uint8_t data[1];

    switch (bitXsample)
    {
    case 16:
        data[0] = 3;
        break;
    case 24:
        data[0] = 5;
        break;
    default:
        return false;
    }

    return this->write_register(DDX_SERIAL_DATA_INTERFACE, data);
}

bool TAS5731::set_registers(const register_data *values, size_t size)
{
    for (uint8_t i = 0; i < size; i++)
    {
        this->write_register(values[i].reg, (uint8_t *)(values[i].values.data()));
    }

    return true;
}

void TAS5731::printallregister()
{
    printregister(DDX_CLOCK_CTL);
    printregister(DDX_DEVICE_ID);
    printregister(DDX_ERROR_STATUS);
    printregister(DDX_SYS_CTL_1);
    printregister(DDX_SERIAL_DATA_INTERFACE);
    printregister(DDX_SYS_CTL_2);
    printregister(DDX_SOFT_MUTE);
    printregister(DDX_MASTER_VOLUME);
    printregister(DDX_CHANNEL1_VOL);
    printregister(DDX_CHANNEL2_VOL);
    printregister(DDX_CHANNEL3_VOL);
    printregister(DDX_VOLUME_CONFIG);

    printregister(DDX_MODULATION_LIMIT);
    printregister(DDX_IC_DELAY_CHANNEL_1);
    printregister(DDX_IC_DELAY_CHANNEL_2);
    printregister(DDX_IC_DELAY_CHANNEL_3);
    printregister(DDX_IC_DELAY_CHANNEL_4);
    printregister(DDX_PWM_SHUTDOWN_GROUP);
    printregister(DDX_START_STOP_PERIOD);
    printregister(DDX_OSC_TRIM);
    printregister(DDX_BKND_ERR);

    printregister(DDX_INPUT_MUX);
    printregister(DDX_CH4_SOURCE_SELECT);

    printregister(DDX_PWM_MUX);

    printregister(DDX_CH1_BQ_0);
    printregister(DDX_CH1_BQ_1);
    printregister(DDX_CH1_BQ_2);
    printregister(DDX_CH1_BQ_3);
    printregister(DDX_CH1_BQ_4);
    printregister(DDX_CH1_BQ_5);
    printregister(DDX_CH1_BQ_6);

    printregister(DDX_CH2_BQ_0);
    printregister(DDX_CH2_BQ_1);
    printregister(DDX_CH2_BQ_2);
    printregister(DDX_CH2_BQ_3);
    printregister(DDX_CH2_BQ_4);
    printregister(DDX_CH2_BQ_5);
    printregister(DDX_CH2_BQ_6);

    printregister(DDX_DRC1_AE);
    printregister(DDX_DRC1_AA);
    printregister(DDX_DRC1_AD);
    printregister(DDX_DRC2_AE);
    printregister(DDX_DRC2_AA);
    printregister(DDX_DRC2_AD);
    printregister(DDX_DRC1_T);
    printregister(DDX_DRC1_K);
    printregister(DDX_DRC1_O);
    printregister(DDX_DRC2_T);
    printregister(DDX_DRC2_K);
    printregister(DDX_DRC2_O);
    printregister(DDX_DRC_CTL);

    printregister(DDX_BANKSWITCH_AND_EQCTL);
    printregister(DDX_CH_1_OUTPUT_MIXER);
    printregister(DDX_CH_2_OUTPUT_MIXER);
    printregister(DDX_CH_1_INPUT_MIXER);
    printregister(DDX_CH_2_INPUT_MIXER);
    printregister(DDX_CH_3_INPUT_MIXER);
    printregister(DDX_OUTPUT_POST_SCALE);
    printregister(DDX_OUTPUT_PRE_SCALE);

    printregister(DDX_CH1_BQ_7);
    printregister(DDX_CH1_BQ_8);
    printregister(DDX_SUBCHANNEL_BQ_0);
    printregister(DDX_SUBCHANNEL_BQ_1);
    printregister(DDX_CH2_BQ_7);
    printregister(DDX_CH2_BQ_8);
    printregister(DDX_PSEUDO_CH2_BQ_0);

    printregister(DDX_CH_4_OUTPUT_MIXER);
    printregister(DDX_CH_4_INPUT_MIXER);
    printregister(DDX_CH_IDF_POST_SCALE);
    printregister(DDX_CH_DEV_ADDR_ENABLE);
    printregister(DDX_CH_DEV_ADDR_UPDATE);
}

void TAS5731::printregister(tas_register reg)
{
    byte *data = read_register(reg);

    Serial.printf("%X => [", reg.address);

    for (uint8_t i = 0; i < reg.size; i++)
    {
        Serial.printf("%X:", data[i]);
    }

    Serial.println("]");
}

byte *TAS5731::read_register(tas_register reg)
{
    byte *buffer = (byte *)malloc(reg.size);

    Wire.beginTransmission((uint8_t)DDX_I2C_ADDR);
    Wire.write((uint8_t)reg.address);
    Wire.endTransmission();
    Wire.requestFrom((uint8_t)DDX_I2C_ADDR, reg.size);
    Wire.readBytes(buffer, reg.size);
    Wire.endTransmission();

    return buffer;
}

bool TAS5731::write_register(tas_register reg, const uint8_t *data)
{
    Wire.beginTransmission((uint8_t)DDX_I2C_ADDR);
    Wire.write((uint8_t)reg.address);
    Wire.write(data, reg.size);
    return Wire.endTransmission() == 0;
}