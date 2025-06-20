#pragma once

#include <iostream>
#include <memory>

#include "Channel.hpp"
#include "Common.hpp"
#include "Connection.hpp"
#include "Epoller.hpp"
#include "Log.hpp"
#include "Reactor.hpp"
#include "Socket.hpp"

using namespace SocketModule;

// Listener 专门进行获取新连接
class Listener : public Connection
{
public:
    Listener(int port = defaultport)
        : _port(port), _listensock(std::make_unique<TcpSocket>())
    {
        _listensock->BuildTcpSocketMethod(_port);
        SetEvent(EPOLLIN | EPOLLET);  // ET todo
        SetNonBlock(_listensock->Fd());
    }
    ~Listener() {}
    void Recver() override
    {
        // accept
        // LOG(LogLevel::DEBUG) << "进入Listener模块的Recver函数";
        InetAddr client;
        // 新连接就绪了，你能保证只有 一个连接到来吗？
        // 一次把所有的连接全部获取上来 while, ET, sockfd设置为非阻塞！！ ----
        // listensock本身设置为非阻塞
        while (true)
        {
            int sockfd = _listensock->Accept(&client);
            if (sockfd == ACCEPT_ERR)
                break;
            else if (sockfd == ACCEPT_CONTINUE)
                continue;
            else if (sockfd == ACCEPT_DONE)
                break;
            else
            {
                // 我们获得的是一个合法的fd，普通的文件描述符
                std::shared_ptr<Connection> conn =
                    std::make_shared<Channel>(sockfd, client);
                conn->SetEvent(EPOLLIN | EPOLLET);
                if (_handler)
                {
                    LOG(LogLevel::DEBUG) << "注册回调方法给channel ";
                    conn->RegisterHandler(_handler);
                }
                else
                {
                    LOG(LogLevel::ERROR) << "回调方法为空, 无法注册";
                }
                GetOwner()->AddConnection(conn);
            }
        }
    }
    void Sender() override {}
    void Excepter() override {}
    int GetSockFd() override { return _listensock->Fd(); }

private:
    int _port;
    std::unique_ptr<Socket> _listensock;
};
