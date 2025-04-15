#include "TcpServer.hpp"

void Usage(std::string proc)
{
    std::cerr << "Usage: " << proc << " port" << std::endl;
}

// ./tcpserver port
int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        Usage(argv[0]);
        exit(USAGE_ERR);
    }
    uint16_t port = std::stoi(argv[1]);

    Enable_Console_Log_Strategy();

    std::unique_ptr<TcpServer> tsvr = std::make_unique<TcpServer>(port);
    tsvr->Init();
    tsvr->Run();

    return 0;
}