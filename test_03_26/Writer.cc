#include "Sem.hpp"
#include <unistd.h>
#include <ctime>
#include <cstdio>

int main()
{
    SemaphoreBuilder sem_builder;
    auto fsem = sem_builder.SetVal(1).Build(BUILD_SEM, 1);

    srand(time(nullptr) ^ getpid());

    pid_t pid = fork();
    // 我们期望的是，父子进行打印的时候，C或者F必须成对出现！必须保证打印是原子的
    if (pid == 0)
    {
        auto csem = sem_builder.Build(GET_SEM);

        while(true)
        {
            csem->P();
            printf("C");
            fflush(stdout);
            usleep(rand() % 95170);
            printf("C");
            fflush(stdout);
            usleep(rand() % 43920);
            csem->V();
        }
        exit(0);
    }
    int cnt = 50;
    while(cnt--)
    {
        fsem->P();
        printf("F");
        fflush(stdout);
        usleep(rand() % 95170);
        printf("F");
        fflush(stdout);
        usleep(rand() % 43920);
        fsem->V();
    }

    return 0;
}