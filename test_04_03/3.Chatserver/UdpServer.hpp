#pragma once
#include <iostream>
#include <string>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "Log.hpp"
#include <functional>
#include "inetAddr.hpp"

using namespace LogModule;

const int defaultfd = -1;

using func_t = std::function<void(int sockfd, const std::string&,InetAddr&)>;

class UdpServer
{
public:
    UdpServer(uint16_t port, func_t func)
        : _sockfd(defaultfd),
        //   _ip(ip),
          _port(port),
          _isrunning(false),
          _fun(func)
    {
    }
    void Init()
    {
        // 1.创建套接字
        _sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        if (_sockfd < 0)
        {
            LOG(LogLevel::FATAL) << "socket error!";
            exit(1);
        }
        LOG(LogLevel::INFO) << "socker success, sockfd: " << _sockfd;

        // 2.绑定socket信息，ip和端口
        // 先填充sockaddr_in结构体
        struct sockaddr_in local;
        bzero(&local, sizeof(local));
        local.sin_family = AF_INET;

        // 本地序列要转换成网络序列
        local.sin_port = htons(_port);
        // ip也是如此，但要注意四字节和字符串（点分十进制）的转换
        // 1.点分十进制转换成4字节，1.4字节在转换成网络序列
        // inet_addr()一个函数完成两项功能
        // local.sin_addr.s_addr = inet_addr(_ip.c_str());

        // 应该绑定到这个宏，这个宏值是0, 不建议绑定具体ip
        // 意思是，凡是发到port端口的报文都交给我，不考虑ip
        // 成为任意地址绑定
        local.sin_addr.s_addr = INADDR_ANY; 
        

        // 为什么服务器端需要显示绑定？
        // 会有很多客户端来访问它，所以服务端的IP和端口是不能轻易改变的
        int n = bind(_sockfd, (struct sockaddr*)&local, sizeof(local));
        if (n < 0)
        {
            LOG(LogLevel::FATAL) << "bind error";
            exit(2);
        }
        LOG(LogLevel::FATAL) << "bind success, sockfd : " << _sockfd;
    }
    void Start()
    {
        _isrunning = true;
        while (_isrunning)
        {
            char buffer[1024];
            struct sockaddr_in peer;
            socklen_t len =  sizeof(peer);
            // 1.收消息
            ssize_t s = recvfrom(_sockfd, buffer, sizeof(buffer)-1, 0, (struct sockaddr*)&peer, &len);
            if (s > 0)
            {
                InetAddr client(peer);

                buffer[s] = 0;
                _fun(_sockfd, buffer, client);
                // LOG(LogLevel::DEBUG) << "-[" << result << "] " << buffer;
                // 2.发消息
                // std::string echo_string = result;
                // echo_string += buffer;
                // sendto(_sockfd, echo_string.c_str(), echo_string.size(), 0, (struct sockaddr*)&peer, len);
            }
            


        }
    }
    ~UdpServer()
    {
    }

private:
    int _sockfd;
    uint16_t _port;
    bool _isrunning;
    func_t _fun;
};