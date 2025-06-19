#pragma once
#include <sys/epoll.h>

#include "Common.hpp"
#include "Log.hpp"

using namespace LogModule;

class Epoller
{
public:
    Epoller() : _epfd(-1)
    {
        _epfd = epoll_create(128);
        if (_epfd < 0)
        {
            LOG(LogLevel::FATAL) << "epoll_create error!";
            exit(EPOLL_CREATE_ERR);
        }
        LOG(LogLevel::INFO) << "epoll_create success, epfd : " << _epfd;
    }

    void AddEvent(int sockfd, uint32_t events)
    {
        struct epoll_event ev;
        ev.events = events;
        ev.data.fd = sockfd;
        int n = epoll_ctl(_epfd, EPOLL_CTL_ADD, sockfd, &ev);
        LOG(LogLevel::DEBUG) << _epfd << " 添加文件描述符, sockfd: " << sockfd;
        if (events & EPOLLIN)
            LOG(LogLevel::DEBUG) << "管理事件EPOLLIN: " << sockfd;
        if (n < 0)
        {
            LOG(LogLevel::ERROR) << "epoll_ctl error: " << strerror(errno)
                                 << " (sockfd=" << sockfd << ")";
            return;
        }
        LOG(LogLevel::INFO) << "epoll_ctl success, sockfd: " << sockfd;
    }

    void DelEvent() {}

    void ModEvent() {}

    int WaitEvents(struct epoll_event *recvs, int recv_num, int timeout)
    {
        int n = epoll_wait(_epfd, recvs, recv_num, timeout);
        if (n < 0)
        {
            LOG(LogLevel::WARNING) << "epoll_wait error";
        }
        else if (n == 0)
        {
            LOG(LogLevel::WARNING) << "epoll_wait timeout";
        }
        else
        {
            // to do
        }
        return n;
    }

    ~Epoller()
    {
        if (_epfd >= 0) close(_epfd);
    }

private:
    int _epfd;
};
