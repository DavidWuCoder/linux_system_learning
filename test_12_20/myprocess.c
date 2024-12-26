#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main()
{
    printf("父进程开始运行，pid: %d\n", getpid());
    pid_t id = fork();
    if (id < 0)
    {
        perror("fork");
        return -1;
    }
    else if (id == 0)
    {
        
        while(1){
            printf("我是一个子进程！我的id是：%d，我父亲的进程id是：%d\n", getpid(), getppid());
            sleep(1);
        }
    }
    else
    {
        while(1){
            printf("我是一个父进程！我的id是：%d，我父亲的进程id是：%d\n", getpid(), getppid());
            sleep(1);
        }
    }
    printf("进程开始运行，pid: %d\n", getpid());
    // chdir("/home/whb");
    // fopen("hello.txt", "a");
    // 
    // while(1){
    //     printf("我是一个进程！我的id是：%d，我父亲的进程id是：%d\n", getpid(), getppid());
    //     sleep(1);
    // }
    // return 0;
}
