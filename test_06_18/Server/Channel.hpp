#pragma once

#include <sys/socket.h>
#include <sys/types.h>

#include <cerrno>
#include <functional>

#include "Common.hpp"
#include "Connection.hpp"
#include "Epoller.hpp"
#include "Log.hpp"
#include "Reactor.hpp"
#include "inetAddr.hpp"

using namespace LogModule;

class Reactor;

#define SIZE 1024

// 封装fd，保证给每个fd一套缓冲
class Channel : public Connection
{
public:
    Channel(int sockfd, InetAddr &client)
        : _sockfd(sockfd), _client_addr(client)
    {
        SetNonBlock(_sockfd);
        SetEvent(EPOLLIN | EPOLLET);
        LOG(LogLevel::DEBUG) << "设置 fd: " << _sockfd << " 为非阻塞";
    }
    void RegisterHandler(handler_t handler) { _handler = handler; }
    void Recver() override
    {
        LOG(LogLevel::DEBUG) << "进入channel模块";
        char buffer[1024];
        while (true)
        {
            buffer[0] = 0;
            int n = recv(_sockfd, buffer, sizeof(buffer) - 1, 0);
            LOG(LogLevel::DEBUG) << "开始运行接收...   ： " << n;
            if (n > 0)
            {
                buffer[n] = 0;
                _inbuffer += buffer;
            }
            else if (n == 0)
            {
                Excepter();
                return;
            }
            else
            {
                if (errno == EAGAIN || errno == EWOULDBLOCK)
                {
                    break;
                }
                else if (errno == EINTR)
                {
                    continue;
                }
                else
                {
                    Excepter();
                }
            }
        }
        if (!_inbuffer.empty())
        {
            LOG(LogLevel::DEBUG) << "开始执行回调函数: \n" << _inbuffer;
            // Channel.cpp
            if (_handler)
            {  // 检查回调是否有效
                _outbuffer += _handler(_inbuffer);
            }
            else
            {
                LOG(LogLevel::ERROR) << "回调函数为空，丢弃数据: " << _inbuffer;
                _inbuffer.clear();  // 避免死循环
            }
        }
        if (!_outbuffer.empty())
        {
            Sender();
            // GetOwner()->EnableReadWrite(_sockfd, true, true);
        }
    }
    void Sender()
    {
        while (true)
        {
            int n = send(_sockfd, _outbuffer.c_str(), _outbuffer.size(), 0);
            if (n > 0)
            {
                _outbuffer.erase(0, n);
                if (_outbuffer.empty())
                {
                    break;
                }
            }
            else if (n == 0)
            {
                break;
            }
            else
            {
                if (errno == EAGAIN || errno == EWOULDBLOCK)
                {
                    break;
                }
                else if (errno == EINTR)
                {
                    continue;
                }
            }
        }
        // 1.数据发送完毕
        // 2. 对方接收缓冲区满了
        if (!_outbuffer.empty())
        {
            // 关心写事件
            GetOwner()->EnableReadWrite(_sockfd, true, true);
        }
        else
        {
            GetOwner()->EnableReadWrite(_sockfd, true, false);
        }
    }
    void Excepter() { GetOwner()->DelConnection(_sockfd); }
    int GetSockFd() { return _sockfd; }

    ~Channel() {}

private:
    int _sockfd;
    std::string _inbuffer;
    std::string _outbuffer;

    // client info
    InetAddr _client_addr;
};
