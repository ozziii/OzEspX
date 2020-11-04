#ifndef TICKER_O_H
#define TICKER_O_H

#ifdef ESP32

extern "C"
{
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
}

typedef void (*callback_with_arg_t)(void *);

class TimerHandleParameter
{
public:
    uint32_t arg;
    callback_with_arg_t callback;
};

#else

#include "c_types.h"
#include "eagle_soc.h"
#include "ets_sys.h"
#include "osapi.h"

static const int ONCE = 0;
static const int REPEAT = 1;

extern "C"
{
    typedef struct _ETSTIMER_ ETSTimer;
}

#endif

class ticker_o
{
public:
    ticker_o() {}

    typedef void (*callback_t)(void);

    typedef void (*callback_with_arg_t)(void *);

    void attach(float seconds, callback_t callback)
    {
        _attach_ms(seconds * 1000, true, reinterpret_cast<callback_with_arg_t>(callback), 0);
    }

    void attach_ms(uint32_t milliseconds, callback_t callback)
    {
        _attach_ms(milliseconds, true, reinterpret_cast<callback_with_arg_t>(callback), 0);
    }

    template <typename TArg>
    void attach(float seconds, void (*callback)(TArg), TArg arg)
    {
        static_assert(sizeof(TArg) <= sizeof(uint32_t), "attach() callback argument size must be <= 4 bytes");
        // C-cast serves two purposes:
        // static_cast for smaller integer types,
        // reinterpret_cast + const_cast for pointer types
        uint32_t arg32 = (uint32_t)arg;
        _attach_ms(seconds * 1000, true, reinterpret_cast<callback_with_arg_t>(callback), arg32);
    }

    template <typename TArg>
    void attach_ms(uint32_t milliseconds, void (*callback)(TArg), TArg arg)
    {
        static_assert(sizeof(TArg) <= sizeof(uint32_t), "attach_ms() callback argument size must be <= 4 bytes");
        uint32_t arg32 = (uint32_t)arg;
        _attach_ms(milliseconds, true, reinterpret_cast<callback_with_arg_t>(callback), arg32);
    }

    void once(float seconds, callback_t callback)
    {
        _attach_ms(seconds * 1000, false, reinterpret_cast<callback_with_arg_t>(callback), 0);
    }

    void once_ms(uint32_t milliseconds, callback_t callback)
    {
        _attach_ms(milliseconds, false, reinterpret_cast<callback_with_arg_t>(callback), 0);
    }

    template <typename TArg>
    void once(float seconds, void (*callback)(TArg), TArg arg)
    {
        static_assert(sizeof(TArg) <= sizeof(uint32_t), "attach() callback argument size must be <= 4 bytes");
        uint32_t arg32 = (uint32_t)(arg);
        _attach_ms(seconds * 1000, false, reinterpret_cast<callback_with_arg_t>(callback), arg32);
    }

    template <typename TArg>
    void once_ms(uint32_t milliseconds, void (*callback)(TArg), TArg arg)
    {
        static_assert(sizeof(TArg) <= sizeof(uint32_t), "attach_ms() callback argument size must be <= 4 bytes");
        uint32_t arg32 = (uint32_t)(arg);
        _attach_ms(milliseconds, false, reinterpret_cast<callback_with_arg_t>(callback), arg32);
    }

    bool isActive()
    {
#ifdef ESP32
        return this->_tiker_is_running;

        //return (xTimerIsTimerActive(this->_ticker) != pdFALSE);
#else
        //TODO change period.
#endif
    }

    void changePeriod_ms(uint32_t milliseconds)
    {
#ifdef ESP32
        if (this->_tiker_is_running)
            xTimerChangePeriod(this->_ticker, pdMS_TO_TICKS(milliseconds), 100);
#else
        //TODO change period.
#endif
    }

    void changePeriod(float seconds)
    {
        this->changePeriod_ms(seconds * 1000);
    }

    void detach()
    {
#ifdef ESP32
        xTimerStop(this->_ticker, 0);
        this->_tiker_is_running = false;
#else
        if (!_timer)
            return;

        os_timer_disarm(_timer);
        delete _timer;
        _timer = nullptr;
#endif
    }

private:
    void _attach_ms(uint32_t milliseconds, bool repeat, callback_with_arg_t callback, uint32_t arg)
    {
#ifdef ESP32
        TimerHandleParameter *newArgs = new TimerHandleParameter();
        newArgs->arg = arg;
        newArgs->callback = callback;

        this->_ticker = xTimerCreate("", pdMS_TO_TICKS(milliseconds), (repeat) ? pdTRUE : pdFALSE, (void *)newArgs, prvCallback);

        xTimerStart(this->_ticker, 0);
        this->_tiker_is_running = true;
#else
        if (_timer)
        {
            os_timer_disarm(_timer);
        }
        else
        {
            _timer = new ETSTimer;
        }

        os_timer_setfn(_timer, reinterpret_cast<ETSTimerFunc *>(callback), reinterpret_cast<void *>(arg));
        os_timer_arm(_timer, milliseconds, (repeat) ? REPEAT : ONCE);
#endif
    }

#ifdef ESP32
    TimerHandle_t _ticker;
    bool _tiker_is_running = false;
    static void prvCallback(TimerHandle_t xTimer)
    {
        TimerHandleParameter *pxStructAddress;
        pxStructAddress = (TimerHandleParameter *)pvTimerGetTimerID(xTimer);
        pxStructAddress->callback((void *)pxStructAddress->arg);
    }
#else
    ETSTimer *_timer;
#endif
};
#endif