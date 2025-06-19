#pragma once

#include <sys/socket.h>
#include <sys/types.h>

#include <cerrno>
#include <functional>

#include "Common.hpp"
#include "Connection.hpp"
#include "Epoller.hpp"
#include "Log.hpp"
#include "inetAddr.hpp"

using namespace LogModule;

class Reactor;

#define SIZE 1024

class Channel;

using handler_t = std::function<void(std::shared_ptr<Channel> channel)>;

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
            _handler(std::shared_ptr<Channel>(this));
        }
    }
    void Sender() {}
    void Excepter() {}
    int GetSockFd() { return _sockfd; }

    ~Channel() {}

private:
    int _sockfd;
    std::string _inbuffer;
    std::string _outbuffer;

    // client info
    InetAddr _client_addr;

    handler_t _handler;
};
