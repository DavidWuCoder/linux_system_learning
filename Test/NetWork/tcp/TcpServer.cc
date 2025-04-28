#include "TcpServer.hpp"

// 命令行启动
// ./tcpserver port 
int main(int argc, char** argv)
{
    if (argc != 2)
    {
        std::cout << "Usage: " << argv[0] << " port" << std::endl;
        exit(1); 
    }

    uint16_t port = std::stoi(argv[1]); 

    // 创建服务器对象
    TcpServer tsvr(port);
    // 初始化和启动服务器
    tsvr.Init();
    tsvr.Start();
    return 0;
}