#include <iostream>
#include <memory>

#include "Common.hpp"
#include "Connection.hpp"
// clang-format off
#include "Reactor.hpp"
#include "Listener.hpp"
// clang-format on

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
    std::unique_ptr<Reactor> R = std::make_unique<Reactor>();

    R->AddConnection(conn);
    R->Loop();

    return 0;
}
