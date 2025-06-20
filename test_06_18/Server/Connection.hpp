#pragma once

#include <sys/types.h>

#include <memory>

#include "Common.hpp"

class Reactor;
class Channel;
using handler_t = std::function<std::string(std::string &)>;

// 封装fd，保证给每个fd一套缓冲
class Connection
{
public:
    Connection() : _events(0), _owner(nullptr) {}
    virtual void Recver() = 0;
    virtual void Sender() = 0;
    virtual void Excepter() = 0;
    virtual int GetSockFd() = 0;

    void SetEvent(const uint32_t &events) { _events = events; }

    uint32_t GetEvents() { return _events; }

    void SetOwner(Reactor *owner) { _owner = owner; }

    void RegisterHandler(handler_t handler) { _handler = handler; }

    Reactor *GetOwner() { return _owner; }

    ~Connection() {}

private:
    // int _sockfd;
    // std::string _inbufer;
    // std::string _outbuffer;

    // 回指指针
    Reactor *_owner;

    // 关心的事件
    uint32_t _events;

protected:
    handler_t _handler;
};
