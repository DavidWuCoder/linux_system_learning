#include "Shm.hpp"

int main()
{
    Shm shm(pathname, projid, CREATER);
    char * mem = (char *)shm.VituralAddr();
    while (true)
    {
        printf("%s\n", mem);
        sleep(1);
    }
    return 0;
}