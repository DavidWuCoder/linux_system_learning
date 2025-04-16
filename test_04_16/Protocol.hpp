#pragma once

#include <string>
#include "Socket.hpp"

using namespace SocketModule;

// client to server
class Request
{
public:
    Request()
    {}

    Request(int x, int y, char oper):_x(x), _y(y), _oper(oper)
    {}

    std::string Serialize()
    {

    }

    bool Deserialize(std::string& in)
    {

    }

    Request()
    {}
private:
    int _x;
    int _y;
    char _oper; // + - * % / -> _x _oper _y
};

// 使用现成的方案，json-> jsoncpp

// server to client
class Response
{
public:
    Response()
    {}

    std::string Serialize()
    {

    }

    bool Deserialize(std::string& in)
    {
        
    }

    ~Response()
    {}
private:
    int _result; // 运算结果
    int _code; // 不同的运算异常的情况
};

class Protocol
{
public:
    Protocol()
    {}

    void GetRequest(std::shared_ptr<Socket> &sock, InetAddr& addr)
    {

    }

    ~Protocol()
    {}


private:
    Request _req;
    Response _resq;
};