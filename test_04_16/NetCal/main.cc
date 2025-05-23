#include "TcpServer.hpp"
#include "Protocol.hpp"
#include "NetCal.hpp"
#include "Daemon.hpp"

using namespace SocketModule;

void Usage(std::string proc)
{
    std::cerr << "Usage: " << proc << " port" << std::endl;
}

// ./tcpserver 808
int main(int argc, char** argv)
{
    if (argc != 2)
    {
        Usage(argv[0]);
        exit(USAGE_ERR);
    }
    std::cout << "服务器已经启动，已经是守护进程了" << std::endl;
    // Daemon(0, 0);
    daemon(0, 0);

    Enable_File_Log_Strategy();

    // 1.顶层
    std::unique_ptr<Cal> cal = std::make_unique<Cal>();

    // 2.协议层
    std::unique_ptr<Protocol> protocol = std::make_unique<Protocol>([&cal](Request &req) -> Response{
        return cal->Execute(req);
    });

    // 3.服务器层
    std::unique_ptr<TcpServer> tsvr = std::make_unique<TcpServer>(std::stoi(argv[1]),
         [&protocol](std::shared_ptr<Socket> &sock, InetAddr &client){
            protocol->GetRequest(sock, client);
    });

    tsvr->Start();

    return 0;
}