#pragma once
#include <iostream>
#include <string>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "Log.hpp"

using namespace LogModule;

const int defaultfd = -1;

class UdpServer
{
public:
    UdpServer(const std::string &ip, uint16_t port)
        : _sockfd(defaultfd),
          _ip(ip),
          _port(port)
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
        local.sin_addr.s_addr = ;

        int n = bind(_sockfd, , );
    }
    void Start()
    {
    }
    ~UdpServer()
    {
    }

private:
    int _sockfd;
    uint16_t _port;
    std::string _ip;
};