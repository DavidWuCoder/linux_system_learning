#include "mystring.h"

int my_strlen(const char* s)
{
    const char* start = s;
    while (*s)
    {
        s++;
    }
    return s - start;
}
