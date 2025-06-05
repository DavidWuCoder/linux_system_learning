#include <iostream>
#include <memory>

#include "SelectServer.hpp"

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cerr << "argv[0]  port" << std::endl;
        return 1;
    }
    Enable_Console_Log_Strategy();
    short port = std::stoi(argv[1]);
    std::unique_ptr<SelectServer> svr = std::make_unique<SelectServer>(port);

    svr->Start();

    return 0;
}
