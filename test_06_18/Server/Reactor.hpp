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
                if (IsConnectionExists(sockfd))
                {
                    _connections[sockfd]->Sender();
                }
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
            PrintMap();
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
        // 0. 不要重复添加
        if (IsConnectionExists(conn))
        {
            LOG(LogLevel::WARNING) << "conn is exists: " << conn->GetSockFd();
            return;
        }
        uint32_t events = conn->GetEvents();
        int sockfd = conn->GetSockFd();
        _epoller_ptr->AddEvent(sockfd, events);

        conn->SetOwner(this);

        _connections[sockfd] = conn;
    }

    void DelConnection(int sockfd)
    {
        // 从epoll中删除
        _epoller_ptr->DelEvent(sockfd);

        // 从_connections中删除
        _connections.erase(sockfd);

        close(sockfd);

        LOG(LogLevel::INFO) << "client quit...";
    }

    void EnableReadWrite(int sockfd, bool readable, bool writeable)
    {
        if (!IsConnectionExists(sockfd))
        {
            LOG(LogLevel::WARNING) << "conn doesn't exists: " << sockfd;
            return;
        }

        // 1.修改关心的shijian
        uint32_t new_event =
            (EPOLLET | (readable ? EPOLLIN : 0) | (writeable ? EPOLLOUT : 0));
        _connections[sockfd]->SetEvent(new_event);

        // 2.写透到内核中
        _epoller_ptr->ModEvent(sockfd, new_event);
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
