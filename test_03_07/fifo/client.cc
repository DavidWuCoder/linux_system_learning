#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "comm.hpp"
#include <unistd.h>

int main()
{
    int fd = open(FIFO_FILE, O_WRONLY);
    if (fd < 0)
    {
        std::cerr << "write fifo error" << std::endl;
        return 1;
    }

    while (true)
    {
        std::cout << "Please Enter# ";
        std::string message;
        std::cin >> message;

        int n = write(fd, message.c_str(), message.size());
        if (n > 0)
        {
        }
    }

    close(fd);
    return 0;
}