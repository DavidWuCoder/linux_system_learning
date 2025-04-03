#include <signal.h>
#include <stdio.h>

#include <csignal>

int main()
{
    sigset_t block_set, old_set;
    sigemptyset(&block_set);
    sigemptyset(&old_set);
    sigaddset(&block_set, 2);
    sigaddset(&block_set, 40);
    sigprocmask(SIG_BLOCK, &block_set, &old_set);
    printf("进程开始运行，pid: %d\n", getpid());
    printf("已经阻塞2号信号和40号信号!\n");
    while (true)
    {
        pause();
    }
    return 0;
}
