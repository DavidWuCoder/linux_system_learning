#pragma once 

#include <iostream>
#include <string>
#include <cstdio>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "Log.hpp"
#include "Common.hpp"

const std::string dev = "/dev/null";

using namespace LogModule;

// 将服务守护进程化的服务
void Daemon(int nochdir ,int noclose)
{
    // 忽略IO，子进程退出等信号
    signal(SIGPIPE, SIG_IGN);
    signal(SIGCHLD, SIG_IGN);

    // 父进程直接结束
    if (fork() > 0)
        exit(0);
    
    // 创建独立的会话
    setsid();

    // 依旧可能关联stdin, stdout, stderr, 需要关闭这些进程
    // 守护进程，不从键盘输入，也不需要向显示器打印
    // 方法1.直接关闭
    // 方法2. 重定向到/dev/null （推荐）

    if (nochdir == 0)
    {
        chdir("/");
    }

    if (noclose == 0)
    {
        int fd = ::open(dev.c_str(), O_RDWR);
        if (fd < 0)
        {
            LOG(LogLevel::FATAL) << "open error" ;
            exit(OPEN_ERR);
        }
        else
        {
            dup2(fd, 0);
            dup2(fd, 1);
            dup2(fd, 2);
            close(fd);
        }
    }
}
