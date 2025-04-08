#include <iostream>
#include <memory>
#include "UdpServer.hpp"

std::string defaulthandler(const std::string&)
{

    return "null";
}

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage:" << argv[0] << "port" << std::endl;
        return 1;
    }
    uint16_t port = std::stoi(argv[1]);
    Enable_Console_Log_Strategy();
    std::unique_ptr<UdpServer> usvr = std::make_unique<UdpServer>(port);
    usvr->Init();
    usvr->Start();
    return 0;
}