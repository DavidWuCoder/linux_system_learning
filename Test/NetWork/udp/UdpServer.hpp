#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstring>
#include <iostream>
#include <string>

const int default_port = 8080;
const int default_sockfd = -1;
const int default_size = 1024;

class UdpServer
{
   public:
    UdpServer() : _port(default_port), _sockfd(default_sockfd)
    {
    }

    // 初始化
    void Init()
    {
        // 创建套接字
        _sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        if (_sockfd == -1)
        {
            std::cerr << "socket error" << std::endl;
            exit(1);
        }
        std::cout << "socket success: " << _sockfd << std::endl;

        // 绑定ip和端口
        struct sockaddr_in local;
        memset(&local, 0, sizeof(local));  // 清零

        // 协议家族、端口号（需转换成网络字节序列（大端））、绑定的ip地址
        local.sin_family = AF_INET;
        local.sin_port = htons(_port);
        local.sin_addr.s_addr = INADDR_ANY;

        // 需要通过bind函数将结构体设置到内核中
        int n = ::bind(_sockfd, (struct sockaddr *)&local, sizeof(local));
        if (n != 0)
        {
            std::cerr << "bind() failed: " << strerror(errno) << std::endl;
            exit(2);
        }
        std::cout << "bind success: " << _port << std::endl;
    }

    // 启动服务器
    void Start()
    {
        char buffer[default_size];
        while (true)
        {
            struct sockaddr_in peer;
            socklen_t len = sizeof(peer);  // 注意类型
            ssize_t n = recvfrom(_sockfd, buffer, sizeof(buffer) - 1, 0,
                                 (struct sockaddr *)&peer, &len);
            if (n > 0)
            {
                buffer[n] = 0;
                // std::cout << "server get: " << buffer << std::endl;
                // // 回显
                // sendto(_sockfd, buffer, strlen(buffer), 0,
                //        (struct sockaddr *)&peer, len);

                char client_ip[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &peer.sin_addr, client_ip, sizeof(client_ip));
                std::cout << "[" << client_ip << ":" << ntohs(peer.sin_port) << "] " << buffer << std::endl;
                sendto(_sockfd, buffer, n, 0, (struct sockaddr*)&peer, len);
            }
        }
    }

    ~UdpServer()
    {
    }

   private:
    int _port;
    std::string ip;
    int _sockfd;
};
