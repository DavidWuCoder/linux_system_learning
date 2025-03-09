#include <iostream>
#include <unistd.h>
#include <signal.h>

void handlerSig(int sig)
{
    std::cout << "获得了一个信号" << sig << std::endl;
}

int main()
{
    signal(SIGINT, handlerSig);
    while (true)
    {
        std::cout << "hello world, pid : " << getpid() << std::endl;
        sleep(1);
    }
    return 0;
}