#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "comm.hpp"
#include <unistd.h>

int main()
{
    umask(0);
    int n = mkfifo(FIFO_FILE, 0666);
    if (n != 0)
    {
        std::cerr << "mkdir fifo error" << std::endl;
        return 1;
    }
    int fd = open(FIFO_FILE, O_RDONLY);
    if (fd < 0)
    {
        std::cerr << "open fifo error" << std::endl;
        return 1;
    }

    char buffer[1024];

    while (true)
    {
        int n = read(fd, buffer, sizeof(buffer) - 1);
        if (n > 0)
        {
            buffer[n] = 0;
            std::cout << "client say# " << buffer << std::endl;
        }
    }

    close(fd);
    return 0;
}