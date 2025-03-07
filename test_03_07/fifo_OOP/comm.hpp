#pragma once

#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define PATH "."
#define FIFO_FILE "fifo"

class NamedFifo
{
public:
    NamedFifo(const std::string &path, const std::string &name)
        : _path(path), _name(name)
    {
        _fifoname = _path + "/" + _name;
        umask(0);
        int n = mkfifo(_fifoname.c_str(), 0666);
        if (n != 0)
        {
            std::cerr << "mkdir fifo error" << std::endl;
        }
    }

    ~NamedFifo()
    {
        // 删除管道文件
        int n = unlink(_fifoname.c_str());
        if (n == 0)
        {
            std::cout << "remove fifo success" << std::endl;
        }
        else
        {
            std::cout << "remove fifo failed" << std::endl;
        }
    }

private:
    std::string _path;
    std::string _name;
    std::string _fifoname;
};

class FileOper
{
public:
    FileOper(const std::string &path, const std::string &name)
        : _path(path), _name(name), _fd(-1)
    {
        _fifoname = _path + "/" + _name;
    }

    void OpenForRead()
    {
        // open
        _fd = open(_fifoname.c_str(), O_RDONLY);
        if (_fd < 0)
        {
            std::cerr << "open fifo error" << std::endl;
            return;
        }
        std::cout << "open fifo success" << std::endl;
    }

    void OpenForWrite()
    {
        // open
        _fd = open(_fifoname.c_str(), O_WRONLY);
        if (_fd < 0)
        {
            std::cerr << "write fifo error" << std::endl;
            return;
        }
        std::cout << "open fifo success" << std::endl;
    }

    void Write()
    {
        // write
        int cnt = 1;
        pid_t id = getpid();
        std::string message;
        while (true)
        {
            std::cout << "Please Enter# ";
            std::getline(std::cin, message);
            message += (", message number: " + std::to_string(cnt++) + ", [" + std::to_string(id) + "]");

            int n = write(_fd, message.c_str(), message.size());
        }
    }

    void Read()
    {
        char buffer[1024];

        // 正常读取
        while (true)
        {
            int n = read(_fd, buffer, sizeof(buffer) - 1);
            if (n > 0)
            {
                buffer[n] = 0;
                std::cout << "client say# " << buffer << std::endl;
            }
            else if (n == 0)
            {
                // TO DO
                std::cout << "client quit!! me too !" << std::endl;
                break;
            }
            else
            {
                std::cerr << "read error!" << std::endl;
                break;
            }
        }
    }

    void Close()
    {
        if (_fd > 0)
            close(_fd);
    }

    ~FileOper()
    {
    }

private:
    std::string _path;
    std::string _name;
    std::string _fifoname;
    int _fd;
};