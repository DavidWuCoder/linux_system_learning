#include <fcntl.h>
#include <unistd.h>

#include <cstdio>
#include <iostream>

// 0 标准输入，默认就是阻塞的
void SetNonBlock(int fd)
{
    int fl = fcntl(fd, F_GETFL);
    if (fl < 0)
    {
        perror("fcntl");
        return;
    }
    fcntl(fd, F_SETFL, fl | O_NONBLOCK);  // O_NONBLOCK：将fd设置为非阻塞
}

int main()
{
    SetNonBlock(0);

    char buffer[1024];
    while (true)
    {
        // Linux中ctrl + d：标识输入结束，read返回值是0，类似读到文件结尾
        ssize_t n = read(0, buffer, sizeof(buffer));
        if (n > 0)
        {
            buffer[n - 1] = 0;
            std::cout << buffer << std::endl;
        }
        else if (
            n <
            0)  // 非阻塞read，如果底层数据没有准备好，数据读取算不算出错？不算
        {
            // 1. 读取出错 2. 底层没有数据准备好
            if (errno == EAGAIN || errno == EWOULDBLOCK)  // 错误码
            {
                std::cout << "数据没有准备好..." << std::endl;
                sleep(1);
                // 做你的事情
                continue;
            }
            else if (errno == EINTR)
            {
                continue;
            }
            else
            {
                // 真正的read出错了
            }
        }
        else
        {
            break;
        }
        //
        // sleep(1);
        // std::cout << ".: " << n << std::endl; // C++也有语言级输出缓冲区
    }
}
