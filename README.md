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
* Download or clone conde in your PlatformIO
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
 * In Your Bowser go to Esp's Ip address
 * Insert default user and password **admin** , **admin**
 * Edit Esp Parameter like **espname** , **User** , **Password** , **WIFI** parameter, **MQTT** parameter. When a paramenter has changed line become bold. When all parameter is ok you can update settings and reboot device.
 * Find your Esp's Ip address and connet.
 * Configure Plug-in
 
         
## Plug-in
Each plug in must be configured with an INITIALIZATION STRING. Initialization string have form like 
**ID / NAME / Param 1 / Param 2 / Param 3 / ...**.

Plug-in represent a new element added to ESP. It can be hardware or software. Some example:  sensors, buttons, led, display, bell ...

In bottom of web gui there are list of all current plug-in with her initialization string format.



* **BINARIY_SENSOR** you can connect with this plug-in a single pin sensor like button, motion sensor (HC-SR501) , Ir Barrier, ecc. this plug in work always in interrupt (on change) mode. If delay parameter is different by 0 it send periodicaly the pin state and when interrup is triggered 
   
   
   Initialization String [#]/[NAME]/[read pin]/[delay (s)]=0/[read logic 1:0]=1/[Pull up 1:0]=0
   * [#]                 Plug-in id
   * [NAME]              A simple name (es. Button1)
   * [read pin]          It represent **GPIO number** of pin where device is connected
   * [delay (s)]=0       Delay in second 
   * [read logic 1:0]=1  
   * [Pull up 1:0]=0
   

* Generic Switch
* Generic Bistable Relay
* DHT22 (temperature and humidity)
* JSN-SR047-V2 (distance)
* PZEM004T (AC current / power)
* Generic radio frequency reciver
* PWM LED White and RGB 
* Nextion Display
* DDNS updater 
    
    
## MQTT 


