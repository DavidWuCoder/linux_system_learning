#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>

void Printpending(sigset_t &pending)
{
    printf("我是一个进程，pid:%d , peding:", getpid());
    for (int signo = 31; signo >= 1; signo--)
    {
        if (sigismember(&pending, signo))
        {
            std::cout << "1";
        }
        else
        {
            std::cout << "0";
        }
    }
    std::cout << std::endl;
}

void handler(int sig)
{
    std::cout << "#######################" << std::endl;
    std::cout << "递达2号信号" << std::endl;
    sigset_t pending;
    int m = sigpending(&pending);
    Printpending(pending);
    std::cout << "#######################" << std::endl;
}

int main()
{
    signal(SIGINT, handler);
    // 1.屏蔽2号信号
    sigset_t block, oblock;
    sigemptyset(&block);
    sigemptyset(&oblock);

    sigaddset(&block, SIGINT);

    // 9号信号不可被捕捉，也不可被屏蔽
    int n = sigprocmask(SIG_SETMASK, &block, &oblock);
    (void)n;

    int cnt = 0;
    while(true)
    {
        // 2.获取pending集合
        sigset_t pending;
        int m = sigpending(&pending);
    
        // 3. 打印pending
        Printpending(pending);
        if (cnt == 10)
        {
            // 恢复
            sigprocmask(SIG_SETMASK, &oblock, nullptr);
            std::cout << "解除对2号的屏蔽" << std::endl;
        }

        sleep(1);
        cnt++;
    }

    return 0;
}

// void handler(int sig)
// {
//     std::cout << "hello sig: " << sig << std::endl;
//     sleep(1);
//     signal(2, SIG_DFL);
// }

// int main()
// {
//     signal(2, handler);
//     // signal(2, SIG_IGN);
//     while (true)
//     {
//         std::cout << "." << std::endl;
//         sleep(1);
//     }
//     return 0;
// }