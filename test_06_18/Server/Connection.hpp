#pragma once

#include <sys/types.h>

#include <string>

#include "inetAddr.hpp"

class TcpServer;

// 封装fd，保证给每个fd一套缓冲
class Connection
{
public:
    Connection() {}
    virtual void Recver() = 0;
    virtual void Sender() = 0;
    virtual void Excepter() = 0;

    void SetEvent(const uint32_t &events) { _events = events; }

    void SetFd(const int &fd) { _sockfd = fd; }

    uint32_t GetEvents() { return _events; }

    int GetFd() { return _sockfd; }

    ~Connection() {}

private:
    int _sockfd;
    std::string _inbufer;
    std::string _outbuffer;

    // 回指指针
    TcpServer *_owner;

    // client info
    InetAddr _client_addr;

    // 关心的事件
    uint32_t _events;
};
