#ifndef SETTING_H
#define SETTING_H

#include <EEPROM.h>

#include <configuration.h>
#include <default_settings.h>
#include <utils.h>

#define SETTING(...) Setting.Get_F(__VA_ARGS__)
#define SETTING_I(s) atoi(SETTING(s).c_str())

#define SETTING_P(...) Setting.Get_P(__VA_ARGS__)
#define SETTING_P_I(s) atoi(SETTING_P(s).c_str())

#define EEPROM_SIZE 1024
#define DATABSE_END_CHAR 35

#define Parameters_Size Size_P(Parameters)

class SettingClass
{
  public:
    SettingClass();

    void begin();

    String Get(String key);

    String Get(const char *key);

    String Get_P(PGM_P format_P, ...);

    String Get_F(const char *format, ...);

    bool Update(String key, String value);

    void RestoreDefault();

    void InitSettings();

  private:
    void Store(Parameter item, uint16_t index);

    String Read(Parameter item, uint16_t index);
};

extern SettingClass Setting;

#endif