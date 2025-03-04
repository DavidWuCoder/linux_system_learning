#include <iostream>
#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <sys/wait.h>

void ChildWrite(int wfd)
{
    char buffer[1024];
    int cnt = 0;
    while (true) 
    {
        snprintf(buffer, sizeof(buffer), "I am a child, pid: %d, cnt: %d", getpid(), cnt++);
        write(wfd, buffer, strlen(buffer));
        sleep(1);
    }
}

void FatherRead(int rfd)
{
    char buffer[1024];
    while (true)
    {
        buffer[0] = 0;
        size_t n = read(rfd, buffer, sizeof(buffer) - 1);
        if (n > 0)
        {
            buffer[n] = 0;
            std::cout << "child say:" <<buffer << std::endl;
        }
    }
}

int main()
{
    // 创建管道
    // 0 : 读 ； 1 ：写
    int fds[2] = {0}; 
    int n = pipe(fds);

    if (n < 0)
    {
        std::cerr << "pipe error" << std::endl;
        return 1;
    }
    std::cout << "fds[0]: " << fds[0] << std::endl;
    std::cout << "fds[1]: " << fds[1] << std::endl;

    // 2.创建子进程
    pid_t id = fork();
    if (id == 0)
    {
        // child
        // 3.关闭不需要的读写端口，形成单向信道
        // 父进程读，子进程写
        close(fds[0]);

        ChildWrite(fds[1]);

        close(fds[1]);
        exit(0);
    }

    // father
    // 3.关闭不需要的读写端口，形成单向信道
    // 父进程读，子进程写
    close(fds[1]);

    FatherRead(fds[0]);
    waitpid(id, nullptr, 0);
    close(fds[0]);
    return 0;
}
