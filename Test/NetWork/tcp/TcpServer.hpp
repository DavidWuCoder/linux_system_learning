#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/wait.h>

const int default_log  = 6;

class TcpServer
{   
public:
    TcpServer(uint16_t port) : _port(port), _isrunning(false)
    {
    }

    // 初始化
    void Init()
    {
        // 1.创建套接字
        _listensock = socket(AF_INET, SOCK_STREAM, 0);
        if (_listensock < 0)
        {
            std::cerr << "socket error" << std::endl;
            exit(1);
        }
        std::cout << "socket success: " << _listensock << std::endl;
        // 2. bind 绑定相关信息
        sockaddr_in local;
        memset(&local, 0, sizeof(local));
        local.sin_family = AF_INET;
        local.sin_addr.s_addr = INADDR_ANY;
        local.sin_port = htons(_port);

        int n = bind(_listensock, (const sockaddr *)&local, sizeof(local));
        if (n < 0)
        {
            std::cerr << "bind error" << std::endl;
            exit(2);
        }
        std::cout << "bind success: " << _port << std::endl;

        // 3. 设置socket为监听状态
        int m = listen(_listensock, default_log);
        if (m < 0)
        {
            std::cerr << "listen error" << std::endl;
            exit(3);
        }
        std::cout << "listen success, sockfd: " << _listensock << std::endl;
    }

        // 服务器将要完成的服务
    // 为方便，我们仅作收发消息的服务，本质上是从文件中IO,支持全双工
    void service(int sockfd, std::string client_ip, uint16_t client_port)
    {
        char buffer[1024];
        // IO
        while (true)
        {
            // 收消息就是在文件中读取
            ssize_t n = read(sockfd, buffer, sizeof(buffer) - 1);
            if (n > 0)
            {
                buffer[n] = 0;
                std::cout << "[" << client_ip << ":" << client_port<< "]#" << buffer << std::endl;
                std::string echo_string = "server say# ";
                echo_string += buffer;
                // 发消息就是向文件中写入
                write(sockfd, echo_string.c_str(), echo_string.size());
            }
            else if(n == 0) // read 返回0表示文件读到了结尾
            {
                std::cout << "client ["<< client_ip << ":" << client_port << "] quit...\n";
                break;
            }
            else
            {
                std::cerr << "read error" << std::endl;
                exit(4);
            }
        }
    }

    // 启动服务器
    void Start()
    {
        signal(SIGCHLD, SIG_IGN); // 忽略掉SIGCHLD信号

        _isrunning = true; // 标志位
        while (_isrunning)
        {
            struct sockaddr_in peer;
            socklen_t len = sizeof(peer);
            int sockfd = accept(_listensock, (sockaddr *)&peer, &len);
            if (sockfd < 0)
            {
                std::cerr << "accept socket error" << std::endl;
                continue;
            }
            std::string client_ip = inet_ntoa(peer.sin_addr);
            uint16_t client_port = ntohs(peer.sin_port);
            std::cout << "get a new link [" << client_ip << "]:" << client_port << std::endl;

            pid_t id = fork();
			if (id == 0){ //child
				//处理请求
				service(sockfd, client_ip, client_port);
				exit(0); //子进程提供完服务退出
			}
        }
    }

private:
    uint16_t _port;
    int _listensock;
    bool _isrunning;
};