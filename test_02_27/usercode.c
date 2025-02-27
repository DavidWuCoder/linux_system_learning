
#include <string.h>
#include <unistd.h>

#include "mystdio.h"
#include "mystring.h"

int main()
{
    MyFile *filep = MyFopen("./log.txt", "a");
    if (!filep)
    {
        printf("fopen error!\n");
        return 1;
    }

    int cnt = 10;
    while (cnt--)
    {
        char *msg = (char *)"hello myfile!!!";
        MyFwrite(filep, msg, strlen(msg));
        MyFFlush(filep);
        printf("buffer: %s\n", filep->outbuffer);
        sleep(1);
    }
    MyFclose(filep);  // FILE *fp

    const char *str = "hello bit!\n";
    printf("strlen: %d\n", my_strlen(str));
    return 0;
}
