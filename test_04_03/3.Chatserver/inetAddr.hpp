#pragma once

#include <iostream>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>

class InetAddr
{
public:
    InetAddr(struct sockaddr_in& addr)
        : _addr(addr)
    {
        _port = ntohs(addr.sin_port);
        _ip = inet_ntoa(addr.sin_addr); // 转换成点分十进制
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