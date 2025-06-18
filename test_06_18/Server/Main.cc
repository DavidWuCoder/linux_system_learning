#include <iostream>
#include <memory>

#include "Common.hpp"
#include "Connection.hpp"
#include "Listener.hpp"
#include "TcpServer.hpp"

using namespace LogModule;

void Usage(std::string proc)
{
    std::cerr << "Usage: " << proc << " _port" << std::endl;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        Usage(argv[0]);
        exit(USAGE_ERR);
    }
    uint16_t port = std::stoi(argv[1]);

    std::shared_ptr<Connection> conn = std::make_shared<Listener>(port);
    std::unique_ptr<TcpServer> tsvr = std::make_unique<TcpServer>();

    tsvr->AddConnection(conn);

    return 0;
}
