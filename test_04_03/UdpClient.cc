#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <cstring>
#include <cstdint>

// ./udpclient server_ip server_port
int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        std::cerr << "Usage:" << argv[0] << "server_ip server_port" << std::endl;
        return 1;
    }
    std::string server_ip = argv[1];
    uint16_t server_port = std::stoi(argv[2]);

    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if (sockfd < 0)
    {
        std::cerr << "socket error" << std::endl;
        return 2;
    }

    struct sockaddr_in server;
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(server_port);
    server.sin_addr.s_addr = inet_addr(server_ip.c_str());

    // 2. 本地的ip和端口是什么？
    // 客户端不需要显示绑定，首次发送消息，操作系统自动给客户端进行绑定。OS知道IP,端口号采用随机IP
    // 为什么不采用显示绑定？ 为了避免多个client端口冲突
    // client的端口号是几不重要，只要是唯一的就行
    while (true)
    {
        std::string input;
        std::cout << "Please Enter# ";
        std::getline(std::cin, input);
        sendto(sockfd, input.c_str(), input.size(), 0, (struct sockaddr*)&server, sizeof(server));

        char buffer[1024];
        struct sockaddr_in peer;
        socklen_t len = sizeof(peer);
        int m = recvfrom(sockfd, buffer, sizeof(buffer)-1, 0, (struct sockaddr*)&peer, &len);
        if (m > 0)
        {
            buffer[m] = 0;
            std::cout << buffer << std::endl;
        }
    }
    return 0;
}