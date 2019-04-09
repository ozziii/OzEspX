#ifndef SYSTEM_O_H
#define SYSTEM_O_H

#include <EEPROM.h>

#ifdef ESP32
#include <esp_pm.h>
#endif

#include <configuration.h>
#include <debug_o.h>
#include <setting.h>
#include <ticker_o.h>

#ifdef STABILITY_CHECK_ENABLE
static void stability_ticker_function()
{
    EEPROM.write(STABILITY_INDEX,0);
    EEPROM.commit(); 
#ifdef DEBUG
    DEBUG_MSG("[SYSTEM] STABILITY CHECK PASSED!! \n");
#endif
}
#endif

class system_o
{
  public:
    system_o() {}

    void begin()
    {
        uint8_t start_time = EEPROM.read(START_TIME); 
        start_time++;
        EEPROM.write(START_TIME,start_time);
        EEPROM.commit(); 

        //analogWriteFreq(100);

#ifdef STABILITY_CHECK_ENABLE
        uint8_t crash_count = EEPROM.read(STABILITY_INDEX);
        crash_count++;
        EEPROM.write(STABILITY_INDEX,crash_count);
        EEPROM.commit(); 
#endif

#ifdef DYNAMIC_FREQUENCY_SCALING
        esp_pm_config_esp32_t pm_config = {
            .max_cpu_freq = MAX_CPU_FREQUENCY,
            .min_cpu_freq = MIN_CPU_FREQUENCY,
            .light_sleep_enable = LIGHT_SLEEP_ENABLED};

        esp_pm_configure(&pm_config);
#endif
    }

    void loop()
    {
        delay(LOOP_DELAY);
    }

    bool checkSystemStability()
    {
#ifdef STABILITY_CHECK_ENABLE
        uint8_t crash_count = EEPROM.read(STABILITY_INDEX);
        if (crash_count > MAX_CRASH_COUNT)
        {
#ifdef DEBUG
            DEBUG_MSG("[SYSTEM] SYSTEM IS UNSTABLE \n");
#endif
            return false;
        }
        else
        {
            stability_ticker.once(STABILITY_CHECK_TIME_S, stability_ticker_function);
#ifdef DEBUG
            DEBUG_MSG("[SYSTEM] START STABILITY CHECK \n");
#endif
            return true;
        }
#else
        return true;
#endif

    }

  private:
    ticker_o stability_ticker;
};

#endif