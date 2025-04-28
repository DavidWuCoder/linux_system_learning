#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <string>

// ./tcpclient server_ip server_port
int main(int argc, char **argv)
{
    if (argc != 3)
    {
        std::cout << "Usage: " << argv[0] << " server_ip server_port" << std::endl;
        exit(1);
    }

    std::string server_ip = argv[1];
    uint16_t port = std::stoi(argv[2]);

    // 1.创建套接字
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        std::cerr << "socket error" << std::endl;
        exit(1);
    }
    std::cout << "socket success: " << sockfd << std::endl;

    // 2.客户端无需绑定自己的端口，由系统自动分配
    // 3.向服务端发起连接
    struct sockaddr_in peer;
    memset(&peer, 0, sizeof(peer));
    peer.sin_addr.s_addr = inet_addr(server_ip.c_str());
    peer.sin_family = AF_INET;
    peer.sin_port = htons(port);

    int n = connect(sockfd, (const sockaddr *)&peer, sizeof(peer));
    if (n < 0)
    {
        std::cout << "connect error" << std::endl;
        exit(2);
    }
    std::cout << "连接服务端 " << "[" << server_ip << ":" << port << "] success!" << std::endl; 

    while (true)
    {
        std::string inbuffer;
        std::cout << "Please Enter# ";
        getline(std::cin, inbuffer);
        ssize_t n = write(sockfd, inbuffer.c_str(),
                          inbuffer.size());
        if (n > 0)
        {
            char buffer[1024];
            ssize_t m = read(sockfd, buffer, sizeof(buffer) - 1);
            if (m > 0)
            {
                buffer[m] = 0;
                std::cout << "get a echo messsge -> " << buffer << std::endl;
            }
            else if (m == 0 || m < 0)
            {
                break;
            }
        }
        else
        {
            break;
        }
    }
    close(sockfd);
    return 0;
}