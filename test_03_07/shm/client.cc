#include "comm.hpp"

int main()
{
    Shm shm;
    // 获取共享内存
    shm.Get();
    sleep(5);
    // 映射到自己的地址空间
    shm.Attach();

    sleep(5);
    return 0;
}