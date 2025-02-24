#include <string.h>
#include <unistd.h>

#include "mystdio.h"

int main()
{
    MYFILE* filep = MyFopen("./log.txt", "a");
    if (!filep)
    {
        printf("fopen err!\n");
        return 1;
    }

    int cnt = 10;
    while (cnt--)
    {
        char* msg = (char*)"hello myfile!!!";
        MyFwrite(filep, msg, strlen(msg));
        MyFFlush(filep);
        printf("buffer: %s\n", filep->outbuffer);
        sleep(1);
    }
    MyFclose(filep);
    return 0;
}
