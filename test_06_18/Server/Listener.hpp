#pragma once

#include <memory>

#include "Common.hpp"
#include "Connection.hpp"
#include "Epoller.hpp"
#include "Socket.hpp"

using namespace SocketModule;

class Listener : public Connection
{
public:
    Listener(int port = defaultport)
        : _port(port), _listensock(std::make_unique<TcpSocket>())
    {
        _listensock->BuildListenSocketMethod(port);
        SetEvent(EPOLLIN);
        SetEvent(_listensock->Fd());
    }
    void Recver() {
        // accept
        //
    };
    void Sender() {};
    void Excepter() {};
    ~Listener() {}

private:
    int _port;
    std::unique_ptr<Socket> _listensock;
};
