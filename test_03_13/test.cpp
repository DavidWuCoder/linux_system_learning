#include <iostream>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

void WaitAll(int num)
{
    while (true) {
        pid_t n = waitpid(-1, nullptr, WNOHANG);
        if (n == 0)
        {
            break;
        }
        else if (n < 0)
        {
            std::cout << "等待失败！！！" << std::endl;
        }
    }
    std::cout << "father process get a signal: " << num << std::endl;
}

int main()
{
    signal(SIGCHLD, WaitAll);
    pid_t id =  fork();
    if (id == 0)
    {
        std::cout << "I am child procss!" << std::endl;
        sleep(3);
        exit(3);
    }
    while(true)
    {
        std::cout << "I am father, exit" << std::endl;
        sleep(1);
    }
    return 0;
}

// volatile int flag = 0;

// void handler(int signu)
// {
//     std::cout << "更改全局变量： " << flag << "-> 1" << std::endl;
//     flag = 1;
// }

// int main()
// {
//     signal(2, handler);

//     while (!flag)
//     {

//     }
//     std::cout << "process quit!" << std::endl;
//     return 0;
// }















// void handler(int signum)
// {
//     std::cout << "hello signal: " << signum << std::endl;
//     sigset_t pending;
//     sigpending(&pending);
//     while(true)
//     {
//         for (int i = 31; i >= 1; --i) // 遍历常见的信号范围
//         {
//             if (sigismember(&pending, i))
//             {
//                 std::cout << "1";
//             }
//             else
//             {
//                 std::cout << "0";
//             }
//         }
//         std::cout << std::endl;
//         sleep(1);
//     }
//     exit(0); // 在处理完信号后退出程序
// }

// int main()
// {
//     struct sigaction act, oact;
//     act.sa_handler = handler; // 使用传统的信号处理函数
//     sigemptyset(&act.sa_mask); // 初始化信号掩码集合
//     sigaddset(&act.sa_mask, SIGQUIT); // 添加 SIGQUIT (信号3)
//     sigaddset(&act.sa_mask, SIGILL); // 添加 SIGILL (信号4)
//     act.sa_flags = 0; // 不启用扩展信号处理

//     // 安装信号处理函数
//     if (sigaction(SIGINT, &act, &oact) == -1) { // 监听 SIGINT (Ctrl+C)
//         perror("sigaction");
//         return 1;
//     }

//     while (true)
//     {
//         std::cout << "hello world: " << getpid() << std::endl;
//         sleep(1);
//     }
//     return 0;
// }

