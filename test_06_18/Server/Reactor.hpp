#pragma once
#include <sys/epoll.h>

#include <memory>
#include <unordered_map>

#include "Connection.hpp"
#include "Epoller.hpp"
#include "Log.hpp"

#define recvs_size 128

class Reactor
{
private:
    bool IsConnectionEmpty() { return _connections.empty(); }

    bool IsConnectionExistsHelper(int sockfd)
    {
        auto iter = _connections.find(sockfd);
        if (iter == _connections.end())
        {
            return false;
        }
        else
        {
            return true;
        }
    }

    bool IsConnectionExists(int sockfd)
    {
        return IsConnectionExistsHelper(sockfd);
    }

    bool IsConnectionExists(std::shared_ptr<Connection> &conn)
    {
        return IsConnectionExistsHelper(conn->GetSockFd());
    }

    int LoopOnce(int timeout)
    {
        return _epoller_ptr->WaitEvents(_recvs, recvs_size, timeout);
    }

    void Dispatcher(int n)
    {
        LOG(LogLevel::DEBUG) << "有文件描述符就绪" << n;
        for (int i = 0; i < n; i++)
        {
            int sockfd = _recvs[i].data.fd;
            uint32_t events = _recvs[i].events;
            // 1. 将所有的异常处理同意转换成IO错误
            if (events & EPOLLERR)
            {
                events |= (EPOLLIN | EPOLLOUT);
            }
            if (events & EPOLLHUP)
            {
                events |= (EPOLLIN | EPOLLOUT);
            }

            if (events & EPOLLIN)
            {
                if (IsConnectionExists(sockfd))
                {
                    _connections[sockfd]->Recver();
                }
            }
            if (events & EPOLLOUT)
            {
                // todo
            }
        }
    }

public:
    Reactor() : _epoller_ptr(std::make_unique<Epoller>()), _isrunning(false) {}

    void Loop()
    {
        if (IsConnectionEmpty()) return;
        _isrunning = true;
        int timeout = -1;
        while (_isrunning)
        {
            int n = LoopOnce(timeout);
            Dispatcher(n);
        }
        _isrunning = false;
    }

    void PrintMap()
    {
        for (auto &[key, value] : _connections)
        {
            LOG(LogLevel::DEBUG) << key << ":" << value->GetSockFd();
        }
    }

    // 该接口要添加新连接到connections中
    void AddConnection(std::shared_ptr<Connection> &conn)
    {
        uint32_t events = conn->GetEvents();
        int sockfd = conn->GetSockFd();
        _epoller_ptr->AddEvent(sockfd, events);

        conn->SetOwner(this);

        _connections[sockfd] = conn;
    }

    ~Reactor() {}

private:
    // 1. Epoll 模型
    std::unique_ptr<Epoller> _epoller_ptr;
    // 2. 是否启动
    bool _isrunning;
    // 3.管理所有的connection，也就是管理所有fd
    std::unordered_map<int, std::shared_ptr<Connection>> _connections;
    // 4.就绪事件数组
    struct epoll_event _recvs[recvs_size];
};
