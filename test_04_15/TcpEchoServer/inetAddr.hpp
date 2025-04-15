#pragma once
#include "Common.hpp"

class InetAddr
{
public:
    InetAddr(struct sockaddr_in& addr)
        : _addr(addr)
    {
        _port = ntohs(addr.sin_port);
        // _ip = inet_ntoa(addr.sin_addr); // 转换成点分十进制
        char ipbuffer[64];
        inet_ntop(AF_INET, &_addr.sin_addr, ipbuffer, sizeof(addr));
        _ip = ipbuffer;
    }

    InetAddr(const std::string &ip, uint16_t port):_ip(ip), _port(port)
    {
        // 主机转网络
        memset(&_addr, 0, sizeof(_addr));
        _addr.sin_family = AF_INET;
        inet_pton(AF_INET, _ip.c_str(), &_addr.sin_addr);
        _addr.sin_port = htons(_port);
        //local.sin_addr.s_addr = inet_addr(_ip.c_str()); // TODO
    }

    InetAddr(uint16_t port = 8080):_port(port),_ip("0")
    {
        // 主机转网络
        memset(&_addr, 0, sizeof(_addr));
        _addr.sin_family = AF_INET;
        _addr.sin_addr.s_addr = INADDR_ANY;
        _addr.sin_port = htons(_port);
        //local.sin_addr.s_addr = inet_addr(_ip.c_str()); // TODO
    }

    std::string Ip()
    {
        return _ip;
    }

    uint16_t Port()
    {
        return _port;
    }

    const struct sockaddr_in &NetAddr() {return _addr;}

    const struct sockaddr* NetAddrPtr() 
    {
        return CONV(_addr);
    }

    socklen_t NetAddrLen()
    {
        return sizeof(_addr);
    }

    bool operator==(const InetAddr& addr)
    {
        return (_ip == addr._ip) && (_port == addr._port);
    }

    std::string StringAddr()
    {
        return _ip + ":" + std::to_string(_port);
    }
private:
    struct sockaddr_in _addr;
    std::string _ip;
    uint16_t _port;
};