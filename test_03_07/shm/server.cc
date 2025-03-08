#include "comm.hpp"

int main()
{
    Shm shm;
    shm.Create();
    shm.Attach();
    shm.VituralAddr();
    sleep(20);
    shm.Destroy();
    return 0;
}