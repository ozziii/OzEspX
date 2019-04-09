#include "setting.h"

SettingClass::SettingClass() {}

void SettingClass::begin()
{
    EEPROM.begin(EEPROM_SIZE);
}

String SettingClass::Get(const char *key)
{
    uint16_t index = EEPROM_BEGIN_INDEX;
    for (int i = 0; i < Parameters_Size; i++)
    {
        Parameter item = Get_CONFIG_P(&Parameters[i]);

        if (strcmp(item.key, key) == 0)
        {
            return Read(item, index);
        }
        else
        {
            index += item.size;
        }
    }
    return "";
}

String SettingClass::Get(String key)
{
    return Get(key.c_str());
}

String SettingClass::Get_P(PGM_P format_P, ...)
{
    char format[strlen_P(format_P) + 1];
    memcpy_P(format, format_P, sizeof(format));

    va_list args;
    va_start(args, format_P);
    char test[1];
    int len = vsnprintf(test, 1, format, args) + 1;
    char *buffer = new char[len];
    vsnprintf(buffer, len, format, args);
    va_end(args);
    String ret = Get(buffer);
    delete[] buffer;
    return ret;
}

String SettingClass::Get_F(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    char test[1];
    int len = vsnprintf(test, 1, format, args) + 1;
    char *buffer = new char[len];
    vsnprintf(buffer, len, format, args);
    va_end(args);
    String ret = Get(buffer);
    delete[] buffer;
    return ret;
}

bool SettingClass::Update(String key, String value)
{
    uint16_t index = EEPROM_BEGIN_INDEX;
    for (int i = 0; i < Parameters_Size; i++)
    {
        Parameter item = Get_CONFIG_P(&Parameters[i]);

        if (key.equals(item.key))
        {
            item.value = value.c_str();
            Store(item, index);
            EEPROM.commit();
            return true;
        }
        else
        {
            index += item.size;
        }
    }

    return false;
}

void SettingClass::RestoreDefault()
{
    EEPROM.write(STABILITY_INDEX, 0);
    EEPROM.write(START_TIME, 0);

    uint16_t index = EEPROM_BEGIN_INDEX;

    for (int i = 0; i < Parameters_Size; i++)
    {
        Parameter item = Get_CONFIG_P(&Parameters[i]);

        Store(item, index);

        index += item.size;
    }

    EEPROM.commit();
}

void SettingClass::Store(Parameter item, uint16_t index)
{
    for (int i = 0; i < strlen(item.value); i++)
    {
        EEPROM.write(index++, item.value[i]);
    }

    if (strlen(item.value) < item.size)
        EEPROM.write(index++, DATABSE_END_CHAR);
}

String SettingClass::Read(Parameter item, uint16_t index)
{
    String value;
    char ee;

    for( int i = 0 ; i < item.size & ( ee = EEPROM.read(index++)) != DATABSE_END_CHAR ; i++ )
    {
        value += ee;
    }
    return value;
}

SettingClass Setting;