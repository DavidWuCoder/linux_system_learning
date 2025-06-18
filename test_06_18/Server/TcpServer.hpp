#include <algorithm>
#include <iostream>
#include <memory>
#include <unordered_map>

#include "Connection.hpp"
#include "Epoller.hpp"
#include "Listener.hpp"

class TcpServer
{
public:
    TcpServer() : _epoller_ptr(std::make_unique<Epoller>()) {}
    void start()
    {
        while (true)
        {
            _epoller_ptr->Wait();
        }
    }
    // 该接口要添加新连接到connections中
    void AddConnection(std::shared_ptr<Connection> &conn)
    {
        uint32_t events = conn->GetEvents();
        int sockfd = conn->GetEvents();
        _epoller_ptr->AddEvent(sockfd, events);

        _connections[sockfd] = conn;
    }
    ~TcpServer() {}

private:
    // 1. Epoll 模型
    std::unique_ptr<Epoller> _epoller_ptr;
    // 3.管理所有的connection，也就是管理所有fd
    std::unordered_map<int, std::shared_ptr<Connection>> _connections;
};
