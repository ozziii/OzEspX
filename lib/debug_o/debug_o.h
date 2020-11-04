#ifndef DEBUG_O_H
#define DEBUG_O_H

#include <Arduino.h>
#include <configuration.h>

#ifdef DEBUG_WEB_SUPPORT
#include <list>
#endif

#define DEBUG_MSG(...) debug::debugSend(__VA_ARGS__)
#define DEBUG_MSG_P(...) debug::debugSend_P(__VA_ARGS__)


#ifdef DEBUG_WEB_SUPPORT
    extern std::list<String> debugList;
#endif

class debug
{
  public:

    static void debugBegin()
    {
#ifdef DEBUG_SERIAL_SUPPORT
        DEBUG_PORT.begin(DEBUG_PORT_BLAUD);
#endif
    }

    static void debugSend(const char *format, ...)
    {
        va_list args;
        va_start(args, format);
        char test[1];
        int len = vsnprintf(test, 1, format, args) + 1;
        char *buffer = new char[len];
        vsnprintf(buffer, len, format, args);
        va_end(args);
        _debugSend(buffer);
        delete[] buffer;
    }

    static void debugSend_P(PGM_P format_P, ...)
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
        _debugSend(buffer);
        delete[] buffer;
    }

  private:
    static void _debugSend(char *message)
    {

#ifdef DEBUG_ADD_TIMESTAMP
        static bool add_timestamp = true;
        char timestamp[10] = {0};
        if (add_timestamp)
            snprintf(timestamp, sizeof(timestamp), "[%06lu] ", millis() % 1000000);
        add_timestamp = (message[strlen(message) - 1] == 10) || (message[strlen(message) - 1] == 13);
#endif

#ifdef DEBUG_SERIAL_SUPPORT
#ifdef DEBUG_ADD_TIMESTAMP
        DEBUG_PORT.printf(timestamp);
#endif
        DEBUG_PORT.printf(message);
#endif

#ifdef DEBUG_WEB_SUPPORT
        debugList.push_back(message);
        if(debugList.size() > MAX_DEBUG_LIST)
        {
            debugList.pop_front();
        }
#endif

    }
};




#endif