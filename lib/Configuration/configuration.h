#ifndef CONFIGURATION_h
#define CONFIGURATION_h

#include <Arduino.h>
#include <driver/i2s.h>


/*===========================================================================
* ============================= GENERAL CONFIGURATION =========================
* ===========================================================================*/

#ifdef ESP32
#define DEVICE_NAME "ESP-WROM-32 DEVKIT V1" // Concatenate both to get a unique device name
#else
#define DEVICE_NAME "ESP8622 LoLin NodeMcu V3"
#endif

#define SW_VERSION "0.7.2 rc"

/*===========================================================================
* ============================= DEBUG CONFIGURATION =========================
* ===========================================================================*/

#define DEBUG

#ifdef DEBUG


#define DEBUG_LOG
#define DEBUG_INFO
#define DEBUG_ERROR


//#define DEBUG_ADD_TIMESTAMP

#define DEBUG_WEB_SUPPORT

#ifdef DEBUG_WEB_SUPPORT
#define MAX_DEBUG_LIST 30
#endif




#define DEBUG_SERIAL_SUPPORT

#ifdef DEBUG_SERIAL_SUPPORT
#define DEBUG_PORT Serial // Default debugging port
#define DEBUG_PORT_BLAUD 115200
#endif

#endif

//#define FORCE_INIT_SETTING

/*===========================================================================
* ============================= SYSTEM CONFIGURATION ========================
* ===========================================================================*/

/////////////// POWERSAVE CONFIGURATION ////////////////

#ifdef ESP32
//#define DYNAMIC_FREQUENCY_SCALING
#ifdef DYNAMIC_FREQUENCY_SCALING
#define MAX_CPU_FREQUENCY RTC_CPU_FREQ_160M
#define MIN_CPU_FREQUENCY RTC_CPU_FREQ_80M
#define LIGHT_SLEEP_ENABLED true
#endif

#endif

#define LOOP_DELAY 300

#define STABILITY_CHECK_ENABLE

#ifdef STABILITY_CHECK_ENABLE
#define STABILITY_CHECK_TIME_S 60
#define MAX_CRASH_COUNT 5
#endif

#define SYSTEM_REBOOT_TIME 0

// Enable OTA update
#define OTA

/*===========================================================================
* ============================= WIFI CONFIGURATION ==========================
* ===========================================================================*/

#define WIFI_AP_STA_DELAY 100000

#define WIFI_AP_SSID "ESPWIFI"
#define WIFI_AP_PASS "ESPWIFIESPWIFIESPWIFI"


// Set Wifi Power to max
#define MAX_POWER_WIFI

/*===========================================================================
* ============================= MQTT CONFIGURATION ==========================
* ===========================================================================*/

#define MQTT_RECONNECT_DELAY_MIN 5000  // Try to reconnect in 5 seconds upon disconnection
#define MQTT_RECONNECT_DELAY_STEP 5000 // Increase the reconnect delay in 5 seconds after each failed attempt
#define MQTT_RECONNECT_DELAY_MAX 120000
#define MQTT_KEEPALIVE 300



//#define MQTT_USE_ASYNC

#define MQTT_TOPIC_SEPARATOR_CHAR 47 

#define MQTT_COMMAND_ACTION "action"
#define MQTT_COMMAND_STATE "state"
#define MQTT_COMMAND_VOLUME "volume"

#define MQTT_SENSOR_TEMPERATURE "temperature"
#define MQTT_SENSOR_HUMIDITY "humidity"
#define MQTT_SENSOR_PRESSURE "pressure"
#define MQTT_SENSOR_DEWPOINT "dewpoint"
#define MQTT_SENSOR_CO "carbonmox"
#define MQTT_SENSOR_CURRENT "current"
#define MQTT_SENSOR_VOLTAGE "voltage"
#define MQTT_SENSOR_POWER "power"
#define MQTT_SENSOR_ENERGY "energy"
#define MQTT_SENSOR_VALUE "value"
#define MQTT_SENSOR_ALIVE "alive"

#define TOPIC_BRIGHTNESS "brightness"
#define TOPIC_RGB "rgb"


// Availability ( LAST WILL )
#define MQTT_AVAILABLE_TOPIC        "available"
#define MQTT_PAYLOAD_AVAILABLE      "online"
#define MQTT_PAYLOAD_NOT_AVAILABLE  "offline"

#define MQTT_STATE_ON "ON"
#define MQTT_STATE_OFF "OFF"


#define MQTT_WILL MQTT_PAYLOAD_NOT_AVAILABLE
#define MQTT_QoS 1
#define MQTT_RETAIN false

/*===========================================================================
* ============================= HARDWARE CONFIGURATION ======================
* ===========================================================================*/

#define DFAULT_PUMP_DELAY 300

#define DEBOUNCING_DELAY 500

#ifndef ESP32
#define MAX_GPIO_NUMBER 10
#endif

#ifdef ESP32
#define MAX_INTERRUPT_SLOT 50
#define MAX_INTERRUPT_TASK_PRIORITY 3

// PWM 
#define PWM_FREQUENCY 5000
#define PWM_RESOLUTION 10
#define PWM_MAX_CHANNEL 15

#endif

#define MIN_SENSOR_DELAY LOOP_DELAY  //millisecond

/*===========================================================================
* ============================= WEB SERVER CONFIGURATION ====================
* ===========================================================================
*
*   WEB COMMAND LIST  IMPORTATN / BEFORE COMMAND!!!
*/

#define WEBSERVER


#ifdef WEBSERVER

#define JSON_DOCUMENT_SIZE  1024

#ifdef ESP32
#define WEBSERVER_START_DELAY  5
#endif

//#define WEB_SECURE

#define COMMAND_HOME "/"
#define COMMAND_HOME_2 "/index"
#define COMMAND_HOME_JSON "/indexj"
//#define COMMAND_DIGITAL_READ "/rdigit"
//#define COMMAND_DIGITAL_WRITE "/wdigit"
//#define COMMAND_DIGITAL_PUMP "/pdigit"
//#define COMMAND_DIGITAL_SWITCH "/sdigit"
//#define COMMAND_ANALOG_READ "/ranalog"
#define COMMAND_RESET "/restart"
#define COMMAND_CONFIG "/config"
#define COMMAND_EDIT_CONFIG "/editconfig"
#define COMMAND_INIT_DATABASE "/defaultsettings"
#define COMMAND_UNLOCK_STABILITY "/stability"
#define COMMAND_MQTT_SELF_SENDER "/mqtt"
//#define COMMAND_DEBUG_PAGE "/debug"
#define COMMAND_DEBUG_JSON "/debugj"

#ifdef OTA 
#define COMMAND_OTA_UPDATYE "/update"
#endif 

/*
*   WEB PARAMETER LIST
*/
//#define PARAM_PIN_NUMBER "pin"
//#define PARAM_PIN_STATE "val"
//#define PARAM_DALAY "delay"
#define PARAM_NAME "name"
#define PARAM_VALUE "val"
#define PARAM_TOPIC "topic"
#define PARAM_MESSAGE "message"

/*
*   WEB RESPOSE 
*/
#define RESP_OK "O"
//#define RESP_HIGHT "H"
//#define RESP_LOW "L"
#define RESP_ERROE "E"
#define RESP_WRONG_PARAMETER "W"

#endif //WEBSERVER

#endif

/*===========================================================================
* ============================= PLUG IN ENABLER =============================
* ===========================================================================
*/

//#define PLUGIN_RADIOFREQUENCY_REVICER
//#define PLUGIN_JSNSR04TV2
//#define PLUGIN_PZEM004T
//#define PLUGIN_NEXTION
#define PLUGIN_SPEAKER
//#define PLUGIN_I2S2UDP
//#define PLUGIN_VMC


/*===========================================================================
* ============================= DISPLAY ====================================
* ===========================================================================*/



#ifdef PLUGIN_NEXTION
#define NEXTION_BLAUD 9600
#endif



/*===========================================================================
* =================   MULTIROOM SPEAKER    ====================================
* ===========================================================================*/

#if defined(PLUGIN_SPEAKER) || defined(PLUGIN_I2S2UDP)
#define SPEAKER_PLUGIN_GENERAL_TOPIC "MULTIROOM/action"
#define SPEAKER_PLUGIN_BUFFER_SIZE 50000
#define SPEAKER_PLUGIN_MULTICAST_PORT 1234
#define SPEAKER_PLUGIN_MULTICAST_IP "239.1.2.3"
#define SPEAKER_PLUGIN_SAMPLE_RATE 44100

//#define SPEAKER_PLUGIN_ASYNC_UDP

#endif

#ifdef PLUGIN_SPEAKER


/***************************************************************
 *     /\
 *     |
 * MAX |-----------------------------*
 *     |                           * '
 *     |                         *   '
 *     |                       *     '
 *     |                     *       '
 *     |                   *         '
 *  1  |-----------------*           '
 *     |             *   '           '
 *     |          *      '           '
 *     |       *         '           '
 * 0--------*------------------------------------->
 *     | LOW_GAIN     MID_GAIN    MAX_GAIN
 *     | 
 * 
 *    [from LOW_GAIN to MID_GAIN]  Y = A1 x + B1
 * 
 *    [from MID_GAIN to MAX_GAIN]  Y = A2 x + B2
 * 
 * 
 * ***************************************************************/

#define SPEAKER_PLUGIN_TOP_GAIN        255
#define SPEAKER_PLUGIN_LOW_GAIN        0

#define SPEAKER_PLUGIN_DEFAULT_GAIN    150

#define SPEAKER_PLUGIN_MIDDLE_GAIN     (SPEAKER_PLUGIN_TOP_GAIN - SPEAKER_PLUGIN_LOW_GAIN)/2
#define SPEAKER_PLUGIN_MAX_GAIN        10

#define SPEAKER_PLUGIN_1_ON_A1_GAIN   SPEAKER_PLUGIN_MIDDLE_GAIN - SPEAKER_PLUGIN_LOW_GAIN
#define SPEAKER_PLUGIN_B1_GAIN        SPEAKER_PLUGIN_A1_GAIN * -SPEAKER_PLUGIN_LOW_GAIN

#define SPEAKER_PLUGIN_A2_GAIN        10
#define SPEAKER_PLUGIN_B2_GAIN        10



#define SPEAKER_PLUGIN_I2S_PORT        I2S_NUM_0
#define SPEAKER_PLUGIN_BUFFER_LENGTHT  1024
#endif

#ifdef PLUGIN_I2S2UDP
#define I2S2UDP_PLUGIN_I2S_PORT    I2S_NUM_1
#define I2S2UDP_BUFFER_LENGTH      1024
#define I2S2UDP_SERIAL             Serial2
#define I2S2UDP_SERIAL_BAUD_RATE   115200
#define I2S2UDP_SERIAL_DELAY       310
#endif




/*===========================================================================
* =================================== VMC ====================================
* ===========================================================================*/
#ifdef PLUGIN_VMC

#define VMC_STATE_AUTO    "auto"
#define VMC_STATE_IN    "in"
#define VMC_STATE_OUT    "out"
#define VMC_STATE_OFF    "off"

#define VMC_PAYLOAD_SEPARATOR_CHAR    44

#endif

