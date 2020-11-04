#ifndef UTILS_h
#define UTILS_h

/*
    PROGMEME UTILS
*/

#include <Arduino.h>
#include <vector>

template <typename T>
void Read_CONFIG_P(const T *sce, T &dest)
{
    memcpy_P(&dest, sce, sizeof(T));
}

template <typename T>
T Get_CONFIG_P(const T *sce)
{
    static T temp;
    memcpy_P(&temp, sce, sizeof(T));
    return temp;
}

template <typename T, size_t N>
size_t Size_P(T (&)[N]) { return N; }

static std::vector<String> splitString(const char *init, char c)
{
    std::vector<String> ret;

    String src = init;

    int i2 = 0;
    int i = src.indexOf(c);

    while (i > 0)
    {
        String item = src.substring(i2, i);
        if (item.length() != 0 && item.indexOf(c) < 0)
        {
            ret.push_back(item);
        }
        i2 = i + 1;
        i = src.indexOf(c, i2);
    }

    String item = src.substring(i2, src.length());
    if (item.length() != 0 && item.indexOf(c) < 0)
    {
        ret.push_back(item);
    }

    return ret;
}

static String stringjoin(const char *one, const char *two)
{
    String stret;
    stret += one;
    stret += two;
    return stret;
}

static String stringjoin(char *one, char *two)
{
    String stret;
    stret += one;
    stret += two;
    return stret;
}

static String stringjoin(char *one, String two)
{
    String stret;
    stret += one;
    stret += two;
    return stret;
}

static String stringjoin(String one, char *two)
{
    String stret;
    stret += one;
    stret += two;
    return stret;
}

static String stringjoin(String one, String two)
{
    String stret;
    stret += one;
    stret += two;
    return stret;
}

#endif