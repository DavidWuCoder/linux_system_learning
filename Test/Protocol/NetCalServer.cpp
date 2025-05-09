#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <pthread.h>



// 网络计算器的线程执行函数, 稍后实现
void* Routine(void* args)
{
    return nullptr;
}


// 服务端
// ./netcal_server port
int main(int argc, char** argv)
{
    // 从命令行接收端口信息
    if (argc != 2)
    {
        std::cerr << "Usage: ./netcal_server port" << std::endl;
        exit(1);
    }
    uint16_t port = std::stoi(argv[1]);

    // 1.创建套接字文件
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0)
    {
        std::cerr << "socket error" << std::endl;
        exit(2);
    }
    std::cout << "socket success" << std::endl;

    // 2.填充结构体信息并绑定
    sockaddr_in local;
    memset(&local, 0, sizeof(local));
    local.sin_addr.s_addr = INADDR_ANY;
    local.sin_family = AF_INET;
    local.sin_port = htons(port);

    int n = bind(fd, (sockaddr *)&local, sizeof(local));
    if (n < 0)
    {
        std::cerr << "bind error" << std::endl;
        exit(3);
    }
    std::cout << "bind success" << std::endl;
    
    // 3.监听
    n = listen(fd, 10);
    if (n < 0)
    {
        std::cerr << "listen error" << std::endl;
        exit(4);
    }
    std::cout << "listen success" << std::endl;


    // 4.启动服务器
    sockaddr_in peer;
    memset(&peer, 0, sizeof(local));
    while (true)
    {
        socklen_t len = sizeof(peer);
        int sock = accept(fd, (sockaddr*)&peer, &len);
        if (sock < 0)
        {
            std::cerr << "accept error" << std::endl;
            continue;
        }
        // 打印接收端的信息
        std::string client_ip = inet_ntoa(peer.sin_addr);
        uint16_t client_port = ntohs(peer.sin_port);
        std::cout << "get a new link [" << client_ip << "]:" << client_port << std::endl;

        pthread_t tid;
        int *p = new int(sock);
        pthread_create(&tid, nullptr, Routine, (void*)p);
    }
    
    return 0;
}