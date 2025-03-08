#include "Shm.hpp"

int main()
{
    Shm shm(pathname, projid, USER);
    char * mem = (char *)shm.VituralAddr();
    for (char c = 'A'; c <= 'Z'; c++)
    {
        mem[c - 'A'] = c;
        sleep(1);
    }
    return 0;
}