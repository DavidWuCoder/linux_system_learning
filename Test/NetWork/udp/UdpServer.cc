#include "UdpServer.hpp"

// ./udpserver 8080
int main(int argc, char** argv)
{
    if (argc != 2)
    {
        std::cout << argv[0] << " port" << std::endl;
        exit(1);
    }

    UdpServer usvr;
    usvr.Init();
    usvr.Start();
    return 0;
}
