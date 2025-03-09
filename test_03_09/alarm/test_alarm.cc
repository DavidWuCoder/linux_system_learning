#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>

// func
void sche()
{
    std::cout << "我是进程调度" << std::endl;
}

void MemManager()
{
    std::cout << "我是周期性的内存管理，正在检查内存问题" << std::endl;
}

void Fflush()
{
    std::cout << "我是刷新程序，正在刷新数据" << std::endl;
}

using func_t = std::function<void()>;

std::vector<func_t> funcs;

int cnt = 0;
void handlerSig(int sig)
{
    std::cout << "####################"<< std::endl;
    for (auto f : funcs)
    {
        f();
    }
    std::cout << "####################"<< std::endl;
    int n = alarm(1);
}

int main()
{
    funcs.push_back(sche);
    funcs.push_back(MemManager);
    funcs.push_back(Fflush);
    signal(SIGALRM, handlerSig);
    alarm(1);
    while (true)
    {
        pause();
    }
    return 0;
}

// int main()
// {
//     signal(SIGALRM, handlerSig);
//     // alarm(1);
//     while (true)
//     {
//         std::cout << "." << std::endl;
//         pause();
//     }
//     return 0;
// }

// int main()
// {
//     signal(SIGALRM, handlerSig);
//     alarm(1);

//     while (true)
//     {
//         // std::cout << "cnt: " << cnt << std::endl;
//         cnt++;
//     }
//     return 0;
// }