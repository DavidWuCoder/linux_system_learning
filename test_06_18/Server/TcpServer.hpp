#include <iostream>
#include <memory>
#include <unordered_map>

#include "Connection.hpp"
#include "Epoller.hpp"
#include "Listener.hpp"

class TcpServer
{
public:
    TcpServer() {}
    ~TcpServer() {}

private:
    // 1. Epoll 模型
    std::unique_ptr<Epoller> _epoller_ptr;
    // 2.listener模块
    std::shared_ptr<Listener> _litenser_ptr;
    // 3.管理所有的connection，也就是管理所有fd
    std::unordered_map<int, std::shared_ptr<Connection>> _connections;
};
