#ifndef DEBUG_O_H
#define DEBUG_O_H

#include <configuration.h>

#ifdef DEBUG

#include <Arduino.h>


#ifdef DEBUG_WEB_SUPPORT
#include <list>
extern std::list<String> debugList;
#endif

static void _debugSend(char *message)
{
#ifdef DEBUG_SERIAL_SUPPORT
    DEBUG_PORT.printf(message);
#endif

#ifdef DEBUG_WEB_SUPPORT
    debugList.push_back(message);
    if (debugList.size() > MAX_DEBUG_LIST)
    {
        debugList.pop_front();
    }
#endif
}

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


#define OZ_DEBUG_MSG(...) debugSend(__VA_ARGS__)
#define OZ_DEBUG_MSG_P(...) debugSend_P(__VA_ARGS__)

#define OZ_LOG_BEGIN() debugBegin()
#else
#define OZ_LOG_BEGIN(){}
#endif


#ifdef DEBUG_ERROR
    #define OZ_LOG_E(...) { OZ_DEBUG_MSG(__VA_ARGS__); }
    #define OZ_LOG_E_P(...) { OZ_DEBUG_MSG_P(__VA_ARGS__); }
#else
    #define OZ_LOG_E(...) {}
    #define OZ_LOG_E_P(...) {}
#endif

#ifdef DEBUG_WARNING
    #define OZ_LOG_W(...) { OZ_DEBUG_MSG(__VA_ARGS__); }
    #define OZ_LOG_W_P(...) { OZ_DEBUG_MSG_P(__VA_ARGS__); }
#else
    #define OZ_LOG_(...) {}
    #define OZ_LOG__P(...) {}
#endif

#ifdef DEBUG_INFO
    #define OZ_LOG_I(...) { OZ_DEBUG_MSG(__VA_ARGS__); }
    #define OZ_LOG_I_P(...) { OZ_DEBUG_MSG_P(__VA_ARGS__); }
#else
    #define OZ_LOG_I(...) {}
    #define OZ_LOG_I_P(...) {}
#endif

#ifdef DEBUG_VERBOSE
    #define OZ_LOG_V(...) { OZ_DEBUG_MSG(__VA_ARGS__); }
    #define OZ_LOG_V_P(...) { OZ_DEBUG_MSG_P(__VA_ARGS__); }
#else
    #define OZ_LOG_V(...) {}
    #define OZ_LOG_V_P(...) {}
#endif




#endif