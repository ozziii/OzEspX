#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include <Arduino.h>
#include <debug_o.h>

#ifdef ESP32
#else
#include <vector>
#endif  

#include <pins.h>

class InterruptsClass
{
  public:
    InterruptsClass()
    {
#ifdef ESP32
        queue = xQueueCreate(MAX_INTERRUPT_SLOT, sizeof(uint8_t));
#endif
    }

    void Push(uint8_t pin)
    {
#ifdef ESP32
        xQueueSend(queue, &pin, portMAX_DELAY);
#else
        buffer.push_back(pin);
#endif
    }

    int8_t Get()
    {
#ifdef ESP32
        uint8_t pin;
        xQueueReceive(queue, &pin, portMAX_DELAY);
        return pin;
#else
        if (buffer.size() == 0)
            return -1;

        noInterrupts();
        std::vector<uint8_t>::iterator first = buffer.begin();
        interrupts();

        uint8_t pin = *first;

        noInterrupts();
        buffer.erase(first);
        interrupts();

        return pin;
#endif
    }

  private:
#ifdef ESP32
    QueueHandle_t queue;
#else
    std::vector<uint8_t> buffer;
#endif
};

extern InterruptsClass Interrupts;


static unsigned long last_trigger_pin00 = 0; static void ICACHE_RAM_ATTR OnInterrupt_00() { if( millis() > last_trigger_pin00 + DEBOUNCING_DELAY ){ last_trigger_pin00 = millis(); Interrupts.Push(0);  } }
static unsigned long last_trigger_pin01 = 0; static void ICACHE_RAM_ATTR OnInterrupt_01() { if( millis() > last_trigger_pin01 + DEBOUNCING_DELAY ){ last_trigger_pin01 = millis(); Interrupts.Push(1);  } }
static unsigned long last_trigger_pin02 = 0; static void ICACHE_RAM_ATTR OnInterrupt_02() { if( millis() > last_trigger_pin02 + DEBOUNCING_DELAY ){ last_trigger_pin02 = millis(); Interrupts.Push(2);  } }
static unsigned long last_trigger_pin03 = 0; static void ICACHE_RAM_ATTR OnInterrupt_03() { if( millis() > last_trigger_pin03 + DEBOUNCING_DELAY ){ last_trigger_pin03 = millis(); Interrupts.Push(3);  } }
static unsigned long last_trigger_pin04 = 0; static void ICACHE_RAM_ATTR OnInterrupt_04() { if( millis() > last_trigger_pin04 + DEBOUNCING_DELAY ){ last_trigger_pin04 = millis(); Interrupts.Push(4);  } }
static unsigned long last_trigger_pin05 = 0; static void ICACHE_RAM_ATTR OnInterrupt_05() { if( millis() > last_trigger_pin05 + DEBOUNCING_DELAY ){ last_trigger_pin05 = millis(); Interrupts.Push(5);  } }
static unsigned long last_trigger_pin06 = 0; static void ICACHE_RAM_ATTR OnInterrupt_06() { if( millis() > last_trigger_pin06 + DEBOUNCING_DELAY ){ last_trigger_pin06 = millis(); Interrupts.Push(6);  } }
static unsigned long last_trigger_pin07 = 0; static void ICACHE_RAM_ATTR OnInterrupt_07() { if( millis() > last_trigger_pin07 + DEBOUNCING_DELAY ){ last_trigger_pin07 = millis(); Interrupts.Push(7);  } }
static unsigned long last_trigger_pin08 = 0; static void ICACHE_RAM_ATTR OnInterrupt_08() { if( millis() > last_trigger_pin08 + DEBOUNCING_DELAY ){ last_trigger_pin08 = millis(); Interrupts.Push(8);  } }
static unsigned long last_trigger_pin09 = 0; static void ICACHE_RAM_ATTR OnInterrupt_09() { if( millis() > last_trigger_pin09 + DEBOUNCING_DELAY ){ last_trigger_pin09 = millis(); Interrupts.Push(9);  } }
static unsigned long last_trigger_pin10 = 0; static void ICACHE_RAM_ATTR OnInterrupt_10() { if( millis() > last_trigger_pin10 + DEBOUNCING_DELAY ){ last_trigger_pin10 = millis(); Interrupts.Push(10); } }
#ifdef ESP32
static unsigned long last_trigger_pin11 = 0; static void ICACHE_RAM_ATTR OnInterrupt_11() { if( millis() > last_trigger_pin11 + DEBOUNCING_DELAY ){ last_trigger_pin11 = millis(); Interrupts.Push(11); } }
static unsigned long last_trigger_pin12 = 0; static void ICACHE_RAM_ATTR OnInterrupt_12() { if( millis() > last_trigger_pin12 + DEBOUNCING_DELAY ){ last_trigger_pin12 = millis(); Interrupts.Push(12); } }
static unsigned long last_trigger_pin13 = 0; static void ICACHE_RAM_ATTR OnInterrupt_13() { if( millis() > last_trigger_pin13 + DEBOUNCING_DELAY ){ last_trigger_pin13 = millis(); Interrupts.Push(13); } }
static unsigned long last_trigger_pin14 = 0; static void ICACHE_RAM_ATTR OnInterrupt_14() { if( millis() > last_trigger_pin14 + DEBOUNCING_DELAY ){ last_trigger_pin14 = millis(); Interrupts.Push(14); } }
static unsigned long last_trigger_pin15 = 0; static void ICACHE_RAM_ATTR OnInterrupt_15() { if( millis() > last_trigger_pin15 + DEBOUNCING_DELAY ){ last_trigger_pin15 = millis(); Interrupts.Push(15); } }
static unsigned long last_trigger_pin16 = 0; static void ICACHE_RAM_ATTR OnInterrupt_16() { if( millis() > last_trigger_pin16 + DEBOUNCING_DELAY ){ last_trigger_pin16 = millis(); Interrupts.Push(16); } }
static unsigned long last_trigger_pin17 = 0; static void ICACHE_RAM_ATTR OnInterrupt_17() { if( millis() > last_trigger_pin17 + DEBOUNCING_DELAY ){ last_trigger_pin17 = millis(); Interrupts.Push(17); } }
static unsigned long last_trigger_pin18 = 0; static void ICACHE_RAM_ATTR OnInterrupt_18() { if( millis() > last_trigger_pin18 + DEBOUNCING_DELAY ){ last_trigger_pin18 = millis(); Interrupts.Push(18); } }
static unsigned long last_trigger_pin19 = 0; static void ICACHE_RAM_ATTR OnInterrupt_19() { if( millis() > last_trigger_pin19 + DEBOUNCING_DELAY ){ last_trigger_pin19 = millis(); Interrupts.Push(19); } }
static unsigned long last_trigger_pin20 = 0; static void ICACHE_RAM_ATTR OnInterrupt_20() { if( millis() > last_trigger_pin20 + DEBOUNCING_DELAY ){ last_trigger_pin20 = millis(); Interrupts.Push(20); } }
static unsigned long last_trigger_pin21 = 0; static void ICACHE_RAM_ATTR OnInterrupt_21() { if( millis() > last_trigger_pin21 + DEBOUNCING_DELAY ){ last_trigger_pin21 = millis(); Interrupts.Push(21); } }
static unsigned long last_trigger_pin22 = 0; static void ICACHE_RAM_ATTR OnInterrupt_22() { if( millis() > last_trigger_pin22 + DEBOUNCING_DELAY ){ last_trigger_pin22 = millis(); Interrupts.Push(22); } }
static unsigned long last_trigger_pin23 = 0; static void ICACHE_RAM_ATTR OnInterrupt_23() { if( millis() > last_trigger_pin23 + DEBOUNCING_DELAY ){ last_trigger_pin23 = millis(); Interrupts.Push(23); } }
static unsigned long last_trigger_pin24 = 0; static void ICACHE_RAM_ATTR OnInterrupt_24() { if( millis() > last_trigger_pin24 + DEBOUNCING_DELAY ){ last_trigger_pin24 = millis(); Interrupts.Push(24); } }
static unsigned long last_trigger_pin25 = 0; static void ICACHE_RAM_ATTR OnInterrupt_25() { if( millis() > last_trigger_pin25 + DEBOUNCING_DELAY ){ last_trigger_pin25 = millis(); Interrupts.Push(25); } }
static unsigned long last_trigger_pin26 = 0; static void ICACHE_RAM_ATTR OnInterrupt_26() { if( millis() > last_trigger_pin26 + DEBOUNCING_DELAY ){ last_trigger_pin26 = millis(); Interrupts.Push(26); } }
static unsigned long last_trigger_pin27 = 0; static void ICACHE_RAM_ATTR OnInterrupt_27() { if( millis() > last_trigger_pin27 + DEBOUNCING_DELAY ){ last_trigger_pin27 = millis(); Interrupts.Push(27); } }
static unsigned long last_trigger_pin28 = 0; static void ICACHE_RAM_ATTR OnInterrupt_28() { if( millis() > last_trigger_pin28 + DEBOUNCING_DELAY ){ last_trigger_pin28 = millis(); Interrupts.Push(28); } }
static unsigned long last_trigger_pin29 = 0; static void ICACHE_RAM_ATTR OnInterrupt_29() { if( millis() > last_trigger_pin29 + DEBOUNCING_DELAY ){ last_trigger_pin29 = millis(); Interrupts.Push(29); } }
static unsigned long last_trigger_pin30 = 0; static void ICACHE_RAM_ATTR OnInterrupt_30() { if( millis() > last_trigger_pin30 + DEBOUNCING_DELAY ){ last_trigger_pin30 = millis(); Interrupts.Push(30); } }
static unsigned long last_trigger_pin31 = 0; static void ICACHE_RAM_ATTR OnInterrupt_31() { if( millis() > last_trigger_pin31 + DEBOUNCING_DELAY ){ last_trigger_pin31 = millis(); Interrupts.Push(31); } }
static unsigned long last_trigger_pin32 = 0; static void ICACHE_RAM_ATTR OnInterrupt_32() { if( millis() > last_trigger_pin32 + DEBOUNCING_DELAY ){ last_trigger_pin32 = millis(); Interrupts.Push(32); } }
static unsigned long last_trigger_pin33 = 0; static void ICACHE_RAM_ATTR OnInterrupt_33() { if( millis() > last_trigger_pin33 + DEBOUNCING_DELAY ){ last_trigger_pin33 = millis(); Interrupts.Push(33); } }
static unsigned long last_trigger_pin34 = 0; static void ICACHE_RAM_ATTR OnInterrupt_34() { if( millis() > last_trigger_pin34 + DEBOUNCING_DELAY ){ last_trigger_pin34 = millis(); Interrupts.Push(34); } }
static unsigned long last_trigger_pin35 = 0; static void ICACHE_RAM_ATTR OnInterrupt_35() { if( millis() > last_trigger_pin35 + DEBOUNCING_DELAY ){ last_trigger_pin35 = millis(); Interrupts.Push(35); } }
static unsigned long last_trigger_pin36 = 0; static void ICACHE_RAM_ATTR OnInterrupt_36() { if( millis() > last_trigger_pin36 + DEBOUNCING_DELAY ){ last_trigger_pin36 = millis(); Interrupts.Push(36); } }
static unsigned long last_trigger_pin37 = 0; static void ICACHE_RAM_ATTR OnInterrupt_37() { if( millis() > last_trigger_pin37 + DEBOUNCING_DELAY ){ last_trigger_pin37 = millis(); Interrupts.Push(37); } }
static unsigned long last_trigger_pin38 = 0; static void ICACHE_RAM_ATTR OnInterrupt_38() { if( millis() > last_trigger_pin38 + DEBOUNCING_DELAY ){ last_trigger_pin38 = millis(); Interrupts.Push(38); } }
static unsigned long last_trigger_pin39 = 0; static void ICACHE_RAM_ATTR OnInterrupt_39() { if( millis() > last_trigger_pin39 + DEBOUNCING_DELAY ){ last_trigger_pin39 = millis(); Interrupts.Push(39); } }
#endif




static void SetInterrups(uint8_t pin, int mode)
{
    mode = pins::convertInterrupMode(mode);

    switch (pin)
    {
    case 0:
        attachInterrupt(digitalPinToInterrupt(pins::convertPin(pin)), OnInterrupt_00, mode);
        break;
    case 1:
        attachInterrupt(digitalPinToInterrupt(pins::convertPin(pin)), OnInterrupt_01, mode);
        break;
    case 2:
        attachInterrupt(digitalPinToInterrupt(pins::convertPin(pin)), OnInterrupt_02, mode);
        break;
    case 3:
        attachInterrupt(digitalPinToInterrupt(pins::convertPin(pin)), OnInterrupt_03, mode);
        break;
    case 4:
        attachInterrupt(digitalPinToInterrupt(pins::convertPin(pin)), OnInterrupt_04, mode);
        break;
    case 5:
        attachInterrupt(digitalPinToInterrupt(pins::convertPin(pin)), OnInterrupt_05, mode);
        break;
    case 6:
        attachInterrupt(digitalPinToInterrupt(pins::convertPin(pin)), OnInterrupt_06, mode);
        break;
    case 7:
        attachInterrupt(digitalPinToInterrupt(pins::convertPin(pin)), OnInterrupt_07, mode);
        break;
    case 8:
        attachInterrupt(digitalPinToInterrupt(pins::convertPin(pin)), OnInterrupt_08, mode);
        break;
    case 9:
        attachInterrupt(digitalPinToInterrupt(pins::convertPin(pin)), OnInterrupt_09, mode);
        break;
    case 10:
        attachInterrupt(digitalPinToInterrupt(pins::convertPin(pin)), OnInterrupt_10, mode);
        break;
#ifdef ESP32
    case 11:
        attachInterrupt(digitalPinToInterrupt(pins::convertPin(pin)), OnInterrupt_11, mode);
        break;
    case 12:
        attachInterrupt(digitalPinToInterrupt(pins::convertPin(pin)), OnInterrupt_12, mode);
        break;
    case 13:
        attachInterrupt(digitalPinToInterrupt(pins::convertPin(pin)), OnInterrupt_13, mode);
        break;
    case 14:
        attachInterrupt(digitalPinToInterrupt(pins::convertPin(pin)), OnInterrupt_14, mode);
        break;
    case 15:
        attachInterrupt(digitalPinToInterrupt(pins::convertPin(pin)), OnInterrupt_15, mode);
        break;
    case 16:
        attachInterrupt(digitalPinToInterrupt(pins::convertPin(pin)), OnInterrupt_16, mode);
        break;
    case 17:
        attachInterrupt(digitalPinToInterrupt(pins::convertPin(pin)), OnInterrupt_17, mode);
        break;
    case 18:
        attachInterrupt(digitalPinToInterrupt(pins::convertPin(pin)), OnInterrupt_18, mode);
        break;
    case 19:
        attachInterrupt(digitalPinToInterrupt(pins::convertPin(pin)), OnInterrupt_19, mode);
        break;
    case 20:
        attachInterrupt(digitalPinToInterrupt(pins::convertPin(pin)), OnInterrupt_20, mode);
        break;
    case 21:
        attachInterrupt(digitalPinToInterrupt(pins::convertPin(pin)), OnInterrupt_21, mode);
        break;
    case 22:
        attachInterrupt(digitalPinToInterrupt(pins::convertPin(pin)), OnInterrupt_22, mode);
        break;
    case 23:
        attachInterrupt(digitalPinToInterrupt(pins::convertPin(pin)), OnInterrupt_23, mode);
        break;
    case 24:
        attachInterrupt(digitalPinToInterrupt(pins::convertPin(pin)), OnInterrupt_24, mode);
        break;
    case 25:
        attachInterrupt(digitalPinToInterrupt(pins::convertPin(pin)), OnInterrupt_25, mode);
        break;
    case 26:
        attachInterrupt(digitalPinToInterrupt(pins::convertPin(pin)), OnInterrupt_26, mode);
        break;
    case 27:
        attachInterrupt(digitalPinToInterrupt(pins::convertPin(pin)), OnInterrupt_27, mode);
        break;
    case 28:
        attachInterrupt(digitalPinToInterrupt(pins::convertPin(pin)), OnInterrupt_28, mode);
        break;
    case 29:
        attachInterrupt(digitalPinToInterrupt(pins::convertPin(pin)), OnInterrupt_29, mode);
        break;
    case 30:
        attachInterrupt(digitalPinToInterrupt(pins::convertPin(pin)), OnInterrupt_30, mode);
        break;
    case 31:
        attachInterrupt(digitalPinToInterrupt(pins::convertPin(pin)), OnInterrupt_31, mode);
        break;
    case 32:
        attachInterrupt(digitalPinToInterrupt(pins::convertPin(pin)), OnInterrupt_32, mode);
        break;
    case 33:
        attachInterrupt(digitalPinToInterrupt(pins::convertPin(pin)), OnInterrupt_33, mode);
        break;
    case 34:
        attachInterrupt(digitalPinToInterrupt(pins::convertPin(pin)), OnInterrupt_34, mode);
        break;
    case 35:
        attachInterrupt(digitalPinToInterrupt(pins::convertPin(pin)), OnInterrupt_35, mode);
        break;
    case 36:
        attachInterrupt(digitalPinToInterrupt(pins::convertPin(pin)), OnInterrupt_36, mode);
        break;
    case 37:
        attachInterrupt(digitalPinToInterrupt(pins::convertPin(pin)), OnInterrupt_37, mode);
        break;
    case 38:
        attachInterrupt(digitalPinToInterrupt(pins::convertPin(pin)), OnInterrupt_38, mode);
        break;
    case 39:
        attachInterrupt(digitalPinToInterrupt(pins::convertPin(pin)), OnInterrupt_39, mode);
        break;    
#endif
    }
}

#ifdef ESP32
static void SetTouchInterrups(uint8_t pin, uint16_t threshold)
{
    switch (pin)
    {
    case T0:
        touchAttachInterrupt(pin, OnInterrupt_04, threshold);
        break;
    case T1:
        touchAttachInterrupt(pin, OnInterrupt_00, threshold);
        break;
    case T2:
        touchAttachInterrupt(pin, OnInterrupt_02, threshold);
        break;
    case T3:
        touchAttachInterrupt(pin, OnInterrupt_15, threshold);
        break;
    case T4:
        touchAttachInterrupt(pin, OnInterrupt_13, threshold);
        break;
    case T5:
        touchAttachInterrupt(pin, OnInterrupt_12, threshold);
        break;
    case T6:
        touchAttachInterrupt(pin, OnInterrupt_14, threshold);
        break;
    case T7:
        touchAttachInterrupt(pin, OnInterrupt_27, threshold);
        break;
    case T8:
        touchAttachInterrupt(pin, OnInterrupt_33, threshold);
        break;
    case T9:
        touchAttachInterrupt(pin, OnInterrupt_32, threshold);
        break;  
    default:
        OZ_LOG_E_P(PSTR("[PINS][ERROR] %d IS NOT VALID PIN FOR TOUCH \n"), pin);
        break;

    }
}

#endif

#endif