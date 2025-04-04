#include <iostream>
#include <memory>
#include "UdpServer.hpp"

int main()
{
    Enable_Console_Log_Strategy();
    std::unique_ptr<UdpServer> usvr = std::make_unique<UdpServer>();
    return 0;
}