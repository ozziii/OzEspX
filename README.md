***UNDER COSTRUCTION....***


First at all very thanks to Xose Pérez for your public code Espurna (https://github.com/xoseperez/espurna).

## OzEspX 

**OzEspX** is a Firmware for **ESP32** and **ESP8622** based board. This Firmware provide to interface from hardware to **MQTT** protocol. Hardware is configurable in runtime, with **Plug-In** logic, in a simple **web UI**. Written for **Arduino IDE** and **PlatformIO**.

## Features

* Work on ESP32 and ESP8266 base board
    * In ESP32 work with FreeRTOS lib
    * ESP8266 work with simple tiker
* Auto search of best Wifi network. if no network was found,it set an Access Point and periodically check for known networks
* Web GUI interface. You can see or edit:
    * Remote reset 
    * Restore default settings
    * Generic board information
    * Parameter Configuration (Username, Password, WIFI, IP, MQTT info )
    * Plug-in Configuration
    * Available Plug-in List
* MQTT
    * Definited MQTT hierarchy
    * MQTT last will for availability  
* Plug-In Configuration.

## Getting started
* Download or clone code to your PlatformIO
* Go to platformio.ini and select your board by un-comment (remove first ";" ) form env_default  line 
   ```ini
         ...
         [platformio]
         ;env_default = nodemcu-lolin          <--- for ESP8266
         ;env_default = esp32doit-devkit-v1    <--- for ESP32
         ...
   ```
 * Compile and Upload 
 * Wait for Esp boot
 * Connect at new WIFI network SSID: "**ESPWIFI**" password: "**ESPWIFIESPWIFIESPWIFI**"  (You can change it in configuration.h)
 * In Your Bowser go to http://192.168.4.1
 * Insert default user and password **admin** , **admin**
 * Edit Esp Parameter like **espname** , **User** , **Password** , **WIFI** parameter, **MQTT** parameter. When a paramenter has changed line become bold. When all parameter is ok you can update settings and reboot device.
 * Find your Esp's Ip address in your local network and connet.
 * Configure Plug-in
 * After all configuratin is ok update settings and reboot device

         
## Plug-in
Plug-in represent a new element added to ESP. It can be hardware or software. Some example:  sensors, buttons, led, display, bell ...

Each plug in must be configured with an INITIALIZATION STRING. Initialization string have form like 
**ID / NAME / Param 1 / Param 2 / Param 3 / ...**.
In bottom of web gui there are list of all current plug-in with her initialization string format.
Each parameter are separated by '/' character. The parameter that end with "]=X" are optional and X is default value. '1:0' stand for binary value 0 or 1 only ammited.


* **BINARIY_SENSOR** you can connect with this plug-in a single pin sensor like button, motion sensor (HC-SR501) , Ir Barrier, ecc. this plug in work always in interrupt (on change) mode. If delay parameter is different by 0 it send also periodicaly the pin state
   
   Initialization String [#]/[NAME]/[read pin]/[delay (s)]=0/[read logic 1:0]=1/[Pull up 1:0]=0
   * [#]                 Plug-in id
   * [NAME]              A simple name (es. Button1)
   * [read pin]          It represent **GPIO number** of pin where device is connected
   * [delay (s)]=0       Delay in second 
   * [read logic 1:0]=1  1 => HIGH = TRUE and LOW = FALSE  // 0 => HIGH = FALSE and LOW = TRUE
   * [Pull up 1:0]=0     0 => NO PULL UP  // 1 => PULL UP
   
   MQTT send topic
   * **Topic:** (espname)/(plug-in name)/state  **Message:** "ON" or "OFF"  Send on interrupt and if delay is set each delay
   
   
* **DHT22** (temperature and humidity) this plug-in read temperature and humidity from a DHT22 sensor easch [delay] second 

   Initialization String [#]/[NAME]/[delay]/[pin]
   * [#]                 Plug-in id
   * [NAME]              A simple name (es. dht_bedroom)
   * [delay]             Delay in second 
   * [pin]               It represent **GPIO number** of pin where device is connected
   
   MQTT send topic
   * **Topic:** (espname)/(plug-in name)/temperature  **Message:** Temperature value in form (xx,xx)
   * **Topic:** (espname)/(plug-in name)/humidity  **Message:** Humidity value in form (xx,xx)


* **PZEM004T** (AC current / power) this plug-in read voltage, current, power and energy from a PZEM004T sensor easch [delay] second 
   
   Initialization String [#]/[NAME]/[delay]/[Hardware(0):Software(1)]/[Serial or Tx pin]=2/[Rx pin]=0
   * [#]                         Plug-in id
   * [NAME]                      A simple name (es. dht_bedroom)
   * [delay]                     Delay in second 
   * [Hardware(0):Software(1)]   O: use Hrdware serial connection 1: Use Software connection
   * [Serial or Tx pin]=2        If Hardware serial set number. If Software serial set Tx **GPIO number** 
   * [Rx pin]=0                  If Software serial set Rx **GPIO number**
   
   MQTT send topic
   * **Topic:** (espname)/(plug-in name)/current  **Message:** Current value in form (xx,xx)
   * **Topic:** (espname)/(plug-in name)/voltage  **Message:** Voltage value in form (xx,xx)
   * **Topic:** (espname)/(plug-in name)/power  **Message:** Istant Power value in form (xx,xx)
   * **Topic:** (espname)/(plug-in name)/energy  **Message:** Energy per hour value in form (xx,xx)


* **SWITCH_PIN_STATE** this plugin  provide to control for a switch by an action pin and an read pin for reading state value. read pin work with in interrupt mode (on change). If [delay] is set, plug-in send state also each [delay] second 
   
   Initialization String [#]/[NAME]/[action pin]/[read pin]/[delay]=0/[action logic 1:0]=1/[read logic 1:0]=1/[start action pin 1:0]=0/[Pull up 1:0]=0
   * [#]                         Plug-in id
   * [NAME]                      A simple name (es. lamp_bedroom)
   * [delay]                     Delay in second 
   * [action logic 1:0]=1        1 => HIGH = TRUE and LOW = FALSE  // 0 => HIGH = FALSE and LOW = TRUE
   * [read logic 1:0]=1          1 => HIGH = TRUE and LOW = FALSE  // 0 => HIGH = FALSE and LOW = TRUE
   * [start action pin 1:0]=0    begin value of action pin 1 = HIGH , 0 = LOW 
   * [Pull up 1:0]=0             read pin pull up mode 0 => NO PULL UP  // 1 => PULL UP
   
   MQTT send topic
   * **Topic:** (espname)/(plug-in name)/state  **Message:** "ON" or "OFF"  
  
   MQTT recive topic
   * **Topic:** (espname)/(plug-in name)/action  **Message:** "ON" or "OFF"
   

* **BISTABLE_RELAY** this plugin  provide to control for a bistable relay by an action pin that send impulse for rele switch and an read pin for reading state value. read pin work with in interrupt mode (on change). If [delay] is set, plug-in send state also each [delay] second 
   
   Initialization String [#]/[NAME]/[action pin]/[read pin]/[delay]=0/[action logic 1:0]=1/[read logic 1:0]=1/[start action pin 1:0]=0/[Pull up 1:0]=0
   * [#]                         Plug-in id
   * [NAME]                      A simple name (es. light_bedroom)
   * [delay]                     Delay in second 
   * [action logic 1:0]=1        NO USEFULL SET IT 1 
   * [read logic 1:0]=1          1 => HIGH = TRUE and LOW = FALSE  // 0 => HIGH = FALSE and LOW = TRUE
   * [start action pin 1:0]=0    begin value of action pin 1 = HIGH , 0 = LOW 
   * [Pull up 1:0]=0             read pin pull up mode 0 => NO PULL UP  // 1 => PULL UP
   
   MQTT send topic
   * **Topic:** (espname)/(plug-in name)/state  **Message:** "ON" or "OFF"  
  
   MQTT recive topic
   * **Topic:** (espname)/(plug-in name)/action  **Message:** "ON" or "OFF"

* **LED DIMMER** this plugin  provide to control single or RGB led by PWM arduino function. 
   
   Initialization String [#]/[NAME]/[min 0:1024]/[max 0:1024]/[Wite pin or R-G-B ]/[Dimmer Logic 1:0] = 1/[On-Off pin]?/[ON-Off Logic 1:0] = 1
   * [#]                         Plug-in id
   * [NAME]                      A simple name (es. tv_led)
   * [min 0:1024]                min PWM value
   * [max 0:1024]                max PWM value
   * [Wite pin or R-G-B ]        If Single Led set **GPIO number**. if is RGB led set **GPIO number red**-**GPIO number green**-**GPIO number blue** (example 4-5-6)
   * [Dimmer Logic 1:0] = 1      Dimmer logic 1 => HIGH TO LOW  0 => LOW TO HIGH
   * [On-Off pin]?               If is present different pin for turn on/off set **GPIO number**. else not add other '/' character
   * [ON-Off Logic 1:0] = 1      1 => HIGH = TRUE and LOW = FALSE  // 0 => HIGH = FALSE and LOW = TRUE
   
   MQTT send topic
   * **Topic:** (espname)/(plug-in name)/state            **Message:** "ON" or "OFF" 
   * **Topic:** (espname)/(plug-in name)/brightnessstate  **Message:**  number 0  to 255 
   * **Topic:** (espname)/(plug-in name)/rgbstate         **Message:**  tre numbers from 0 to 255 in format "r,g,b" 
  
   MQTT recive topic
   * **Topic:** (espname)/(plug-in name)/action            **Message:** "ON" or "OFF"
   * **Topic:** (espname)/(plug-in name)/brightnessaction  **Message:**  number 0  to 255 
   * **Topic:** (espname)/(plug-in name)/rgbaction         **Message:**  tre numbers from 0 to 255 in format "r,g,b"
   
   
* **SWITCH_SELF_STATE** ..... 
   
   Initialization String [#]/[NAME]/[action pin]/[delay]=0/[action logic 1:0]=1/[start action pin 1:0]=0
   
* **NEXTION** ..... 
   
   Initialization String [#]/[NAME]/[delay (ms)]/[Hardware Serial]
   
* **RF_Rx_Switch** ..... 
   
   Initialization String [#]/[NAME]/[sensor pin]/[switch pin]/[relay logic 1:0]/[valid key]
  
* **MORE PLUG-IN COMING SOON....**
