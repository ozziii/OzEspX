// #include "configuration.h" //TODO REMOVE

#ifdef NEXTION
#ifndef NEXTION_PLUGIN
#define NEXTION_PLUGIN

#include "plugin_base.h"
#include "plugin_response.h"
#include "plugin_sensor.h"

#include <Nextion.h>

/******************************************************************************************************************************************
*            
*                 NEXTION DISPLAY HMI PLUGIN  (Only hardware serial allowed)
*
*     Init String:  {[*]/[NAME]/[delay (ms)]/[Hardware Serial]}
*
*********************************************************************************************************************************************/

struct Pair
{
  const char *topic;
  const char *command;
  const char *message;
};

static const Pair GuiFiller[] = {
    {"CAMERA/DHT22/temperature", "t0.txt=\"%s\"", ""},
    {"CAMERA/LED/state", "p0.pic=1", MQTT_STATE_ON},
    {"CAMERA/LED/state", "p0.pic=0", MQTT_STATE_OFF},
};

static const Pair GuiCommand[] = {
    {"CAMERA/LED/action", "65 0 1 0 ff ff ff", MQTT_STATE_ON},
    {"CAMERA/LED/action", "65 0 2 0 ff ff ff", MQTT_STATE_OFF},
};

class nextion_plugin : public plugin_base, public plugin_response, public plugin_sensor
{

public:
  nextion_plugin(const char *init)
  {

    std::vector<String> config = splitString(init, PLUGIN_INIT_SEPARATOR_CHAR);

    if (config.size() == 4)
    {
      name = config[1];
      sensor_delay = atol(config[2].c_str());

      uint8_t serial = atoi(config[3].c_str());
      _serial = new HardwareSerial(serial);
      myNextion = new Nextion(*_serial, NEXTION_BLAUD);

      initialized = true;

#ifdef DEBUG
      DEBUG_MSG_P(PSTR("[NEXTION][%s] IS INITIALZED DELAY %d ms \n"), name.c_str(), sensor_delay);
#endif

      //subscribe MQTT ACTION
      for (int i = 0; i < Size_P(GuiFiller); i++)
      {
        Network.subscribe(GuiFiller[i].topic);
      }
    }
  }

  // RECIVE MQTT TO UPDATE GUI
  bool sendResponse(String topic, String Message) override
  {
    bool ret = false;

    for (int i = 0; i < Size_P(GuiFiller); i++)
    {
      if (topic.equals(GuiFiller[i].topic))
      {
        if (strlen(GuiFiller[i].message) == 0 || Message.equals(GuiFiller[i].message))
        {
          char *command = new char[20];
          sprintf(command, GuiFiller[i].command, Message.c_str());
          myNextion->sendCommand(command);

#ifdef DEBUG
          DEBUG_MSG_P(PSTR("[NEXTION][%s] Arrive MQTT (%s) message (%s) \n"), name.c_str(), GuiFiller[i].topic, Message.c_str());
          DEBUG_MSG_P(PSTR("[NEXTION][%s] Send command (%s) \n"), name.c_str(), command);
#endif
          delete[] command;

          ret = true;
          break;
        }
      }
    }

    return ret;
  }

  // EXECUTE COMMAND FORM DISPLAY
  void execute_sensor() override
  {
    String command = myNextion->listen();
    if (command.length() > 0)
    {
#ifdef DEBUG
      DEBUG_MSG_P(PSTR("[NEXTION][%s] incoming command (%s) \n"), name.c_str(), command.c_str());
#endif
      for (int i = 0; i < Size_P(GuiCommand); i++)
      {
        if (command.equals(GuiCommand[i].command))
        {
          Network.send(GuiCommand[i].topic, GuiCommand[i].message);
#ifdef DEBUG
          DEBUG_MSG_P(PSTR("[NEXTION][%s] incoming command (%s) \n"), name.c_str(), command.c_str());
          DEBUG_MSG_P(PSTR("[NEXTION][%s] Send MQTT topic (%s) message (%s) \n"), name.c_str(), GuiCommand[i].topic, GuiCommand[i].message);
#endif
          break;
        }
      }
    }
  }

  static const char *ClassName() { return "NEXTION"; }

private:
  HardwareSerial *_serial = NULL;
  Nextion *myNextion = NULL;
};
#endif
#endif