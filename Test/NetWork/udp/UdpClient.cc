#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstring>
#include <iostream>
#include <string>

// ./udpclient server_ip server_port
int main(int argc, char** argv)
{
    if (argc != 3)
    {
        std::cout << "Usage: ./udpclient server_ip server_port";
        return 1;
    }

    std::string server_ip = argv[1];
    int port = std::stoi(argv[2]);
    std::cout << server_ip <<  ' '<< port << std::endl;

    // 1. 创建socket
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
    {
        if (sock == -1)
        {
            std::cerr << "socket error" << std::endl;
            exit(1);
        }
    }
    // 2. 客户端不需要手动bind,由系统随机分配端口
    // 填充server信息
    struct sockaddr_in server;
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_addr(server_ip.c_str());

    // 3. 收发消息
    while (true)
    {
        std::string inbuffer;
        std::cout << "Please Enter#";
        std::getline(std::cin, inbuffer);

        ssize_t n = sendto(sock, inbuffer.c_str(), inbuffer.size(), 0,
                           (struct sockaddr*)&server, sizeof(server));
        // 发送成功，回显
        if (n > 0)
        {
            // 3. 接受消息
            char buffer[1024];
            sockaddr_in peer;
            socklen_t len = sizeof(peer);
            int n = recvfrom(sock, buffer, sizeof(buffer) - 1, 0,
                             (struct sockaddr*)&peer, &len);
            if (n > 0)
            {
                buffer[n] = 0;
                std::cout << "server_say# : " << buffer << std::endl;
            }
            else
            {
                break;
            }
        }
        else
        {
            break;
        }
    }
    close(sock);
    return 0;
}
