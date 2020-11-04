#ifndef _TAS5731_H
#define _TAS5731_H

#include <Arduino.h>
#include <vector>
#include <Wire.h>

#define DDX_I2C_ADDR 0x1B

struct tas_register
{
       byte address;
       byte size;
};

const tas_register DDX_CLOCK_CTL{0x00, 1};
const tas_register DDX_DEVICE_ID{0x01, 1};
const tas_register DDX_ERROR_STATUS{0x02, 1};
const tas_register DDX_SYS_CTL_1{0x03, 1};
const tas_register DDX_SERIAL_DATA_INTERFACE{0x04, 1};
const tas_register DDX_SYS_CTL_2{0x05, 1};
const tas_register DDX_SOFT_MUTE{0x06, 1};
const tas_register DDX_MASTER_VOLUME{0x07, 1};
const tas_register DDX_CHANNEL1_VOL{0x08, 1};
const tas_register DDX_CHANNEL2_VOL{0x09, 1};
const tas_register DDX_CHANNEL3_VOL{0x0A, 1};
const tas_register DDX_VOLUME_CONFIG{0x0E, 1};

const tas_register DDX_MODULATION_LIMIT{0x10, 1};
const tas_register DDX_IC_DELAY_CHANNEL_1{0x11, 1};
const tas_register DDX_IC_DELAY_CHANNEL_2{0x12, 1};
const tas_register DDX_IC_DELAY_CHANNEL_3{0x13, 1};
const tas_register DDX_IC_DELAY_CHANNEL_4{0x14, 1};
const tas_register DDX_PWM_SHUTDOWN_GROUP{0x19, 1};
const tas_register DDX_START_STOP_PERIOD{0x1A, 1};
const tas_register DDX_OSC_TRIM{0x1B, 1};
const tas_register DDX_BKND_ERR{0x1C, 1};

const tas_register DDX_INPUT_MUX{0x20, 4};
const tas_register DDX_CH4_SOURCE_SELECT{0x21, 4};

const tas_register DDX_PWM_MUX{0x25, 4};

const tas_register DDX_CH1_BQ_0{0x29, 20};
const tas_register DDX_CH1_BQ_1{0x2A, 20};
const tas_register DDX_CH1_BQ_2{0x2B, 20};
const tas_register DDX_CH1_BQ_3{0x2C, 20};
const tas_register DDX_CH1_BQ_4{0x2D, 20};
const tas_register DDX_CH1_BQ_5{0x2E, 20};
const tas_register DDX_CH1_BQ_6{0x2F, 20};

const tas_register DDX_CH2_BQ_0{0x30, 20};
const tas_register DDX_CH2_BQ_1{0x31, 20};
const tas_register DDX_CH2_BQ_2{0x32, 20};
const tas_register DDX_CH2_BQ_3{0x33, 20};
const tas_register DDX_CH2_BQ_4{0x34, 20};
const tas_register DDX_CH2_BQ_5{0x35, 20};
const tas_register DDX_CH2_BQ_6{0x36, 20};

const tas_register DDX_DRC1_AE{0x3A, 8};
const tas_register DDX_DRC1_AA{0x3B, 8};
const tas_register DDX_DRC1_AD{0x3C, 8};
const tas_register DDX_DRC2_AE{0x3D, 8};
const tas_register DDX_DRC2_AA{0x3E, 8};
const tas_register DDX_DRC2_AD{0x3F, 8};
const tas_register DDX_DRC1_T{0x40, 4};
const tas_register DDX_DRC1_K{0x41, 4};
const tas_register DDX_DRC1_O{0x42, 4};
const tas_register DDX_DRC2_T{0x43, 4};
const tas_register DDX_DRC2_K{0x44, 4};
const tas_register DDX_DRC2_O{0x45, 4};
const tas_register DDX_DRC_CTL{0x46, 4};

const tas_register DDX_BANKSWITCH_AND_EQCTL{0x50, 4};
const tas_register DDX_CH_1_OUTPUT_MIXER{0x51, 12};
const tas_register DDX_CH_2_OUTPUT_MIXER{0x52, 12};
const tas_register DDX_CH_1_INPUT_MIXER{0x53, 16};
const tas_register DDX_CH_2_INPUT_MIXER{0x54, 16};
const tas_register DDX_CH_3_INPUT_MIXER{0x55, 12};
const tas_register DDX_OUTPUT_POST_SCALE{0x56, 4};
const tas_register DDX_OUTPUT_PRE_SCALE{0x57, 4};

const tas_register DDX_CH1_BQ_7{0x58, 20};
const tas_register DDX_CH1_BQ_8{0x59, 20};
const tas_register DDX_SUBCHANNEL_BQ_0{0x5A, 20};
const tas_register DDX_SUBCHANNEL_BQ_1{0x5B, 20};
const tas_register DDX_CH2_BQ_7{0x5C, 20};
const tas_register DDX_CH2_BQ_8{0x5D, 20};
const tas_register DDX_PSEUDO_CH2_BQ_0{0x5E, 20};

const tas_register DDX_CH_4_OUTPUT_MIXER{0x60, 8};
const tas_register DDX_CH_4_INPUT_MIXER{0x61, 8};
const tas_register DDX_CH_IDF_POST_SCALE{0x62, 4};
const tas_register DDX_CH_DEV_ADDR_ENABLE{0xF8, 4};
const tas_register DDX_CH_DEV_ADDR_UPDATE{0xF9, 4};

struct register_data
{
       tas_register reg;
       std::vector<char> values;
};



class TAS5731
{
public:
       /**
         *  @brief 
         * 
         */
       TAS5731(uint8_t RESET, uint8_t PDN, uint8_t PVDD, bool PVDD_LOGIC, int SDA, int SCL);
       /**
         *  @brief 
         * 
         */
       TAS5731(uint8_t RESET, uint8_t PDN, uint8_t PVDD, bool PVDD_LOGIC);

       /**
         *  @brief 
         * 
         */
       void Initialization();

       /**
         *  @brief 
         * 
         */
       void Exit_Shutdown();

       /**
         *  @brief 
         * 
         */
       void Enter_Shutdown();

       /**
         *  @brief 
         * 
         */
       void Power_Down();

       /**
         *  @brief Set Master volume  
         * 
         *  @param volume 0 = mute ; 255 = MAX
         * 
         *  @return false if an error occurred
         */
       bool setVolume(uint8_t volume);

       /**
         * @brief Set bit per sample in i2s and IC 
         * 
         * @param bitXsample  only acepted 16 / 24
         * 
         * @return false if an error occurred
         */
       bool set_bit_per_sample(uint8_t bitXsample);

       /**
         * @brief Set group of register
         * 
         * @param values  list of register_data value
         * 
         * @param size  number of register to set
         * 
         * @return false if an error occurred
         */
       bool set_registers(const register_data *values, size_t size);

       // TO REMOVE
       void printallregister();
       void printregister(tas_register reg);

protected:
       uint8_t _reset_pin, _pdn_pin, _pvdd_pin;
       bool _pvdd_logic;
       byte *read_register(tas_register reg);
       bool write_register(tas_register reg, const uint8_t *data);
};

#endif