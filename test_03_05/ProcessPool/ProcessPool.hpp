#ifndef PROCESS_POOL_HPP
#define PROCESS_POOL_HPP

#include <iostream>
#include <cstdlib>
#include <vector>
#include <unistd.h>
#include <sys/wait.h>
#include "Task.hpp"

class Channel
{
public:
    Channel(int fd, pid_t id)
     :_wfd(fd), _subid(id)
    {
        _name = "channel-" + std::to_string(_wfd) + "-" + std::to_string(_subid);
    }

    ~Channel()
    {}

    void Close()
    {
        close(_wfd);
    }

    void Wait()
    {
        pid_t rid = waitpid(_subid, nullptr, 0);
        (void)rid;
    }

    void Send(int code)
    {
        int n = write(_wfd, &code, sizeof(code));
        (void)n;
    }
    int Fd() {return _wfd;}
    pid_t SubID() {return _subid;}
    std::string Name() {return _name;}

private:
    int _wfd;
    pid_t _subid;
    std::string _name;
};

class ChannelManager
{
public:
    ChannelManager():_next(0) {}

    void InsertChannel(int wfd, pid_t subid)
    {
        _channels.emplace_back(wfd, subid);
        // Channel c(wfd, subid);
        // _channels.push_back(c);
    }

    Channel& select()
    {
        auto &c = _channels[_next];
        _next++;
        _next %= _channels.size();
        return c;
    }

    void PrintChannel()
    {
        for (auto& channel : _channels)
        {
            std::cout << channel.Name() << std::endl;
        }
    }

    void StopSubProcess()
    {
        for (auto& c : _channels)
        {
            c.Close();
            std::cout << "关闭：" << c.Name() << std::endl;
        }
    }

    void WaitSubProcess()
    {
        for (auto& c : _channels)
        {
            c.Wait();
            std::cout << "回收：" << c.Name() << std::endl;
        }
    }

    ~ChannelManager() {}

private:
    std::vector<Channel> _channels;
    int _next;
};

const int gdefaultnum = 5;

class ProcessPool
{
public:
    ProcessPool(int num)
        :_process_num(num)
    {
        _tm.Register(PrintLog);
        _tm.Register(Download);
        _tm.Register(Upload);
    }

    void Work(int rfd)
    {
        while (true)
        {
            int code = 0;
            ssize_t n = read(rfd, &code, sizeof(code));

            if (n > 0)
            {
                if (n != sizeof(code))
                {
                    continue;
                }
                std::cout << "子进程["<< getpid() << "]收到一个任务码：" << code << std::endl;
                _tm.Excute(code);
            }
            else if (n == 0)
            {
                std::cout << "子进程退出" <<  std::endl;
                break;
            }
            else
            {
                std::cout << "读取错误" << std::endl;
                break;
            }
        }
    }

    bool Start()
    {
        for (int i = 0; i < _process_num; i++)
        {
            // 1.创建管道
            int pipefd[2] = {0};
            int n = pipe(pipefd);
            if (n < 0) return false;

            // 2.创建子进程
            pid_t subid = fork();
            if (subid < 0) return false;
            if (subid == 0)
            {
                // 子进程
                close(pipefd[1]);
                Work(pipefd[0]);
                exit(0);
            }
            else
            {
                // 父进程
                close(pipefd[0]);

                _cm.InsertChannel(pipefd[1], subid);
                //wfd, subid
            }
        }
        return true;
    }

    void Debug()
    {
        _cm.PrintChannel();
    }

    void Run()
    {
        // 1.选择一个任务
        int task_code = _tm.Code();

        // 2.负载均衡地选取一个子进程
        auto& c = _cm.select();
        std::cout << "选择一个子进程：" << c.Name() << std::endl;

        // 3.发送任务
        c.Send(task_code);
        std::cout << "发送了一个任务码" << task_code << std::endl;
    }

    void Stop()
    {
        // 关闭父进程的wfd即可
        _cm.StopSubProcess();
        // 回收所有子进程
        _cm.WaitSubProcess();
    }

    ~ProcessPool()
    {

    }
private:
    ChannelManager _cm;
    int _process_num;
    TaskManager _tm;
};

#endif