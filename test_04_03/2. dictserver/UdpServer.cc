#include <iostream>
#include <memory>
#include <fstream> 
#include "Dict.hpp"
#include "UdpServer.hpp"

std::string defaulthandler(const std::string& )
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

    // 1.字典对象提供翻译功能
    Dict dict;
    dict.LoadDict();

    // 2.网络提供通信功能
    std::unique_ptr<UdpServer> usvr = std::make_unique<UdpServer>(port,
         [&dict](const std::string& word) -> std::string{
            return dict.Translate(word);
    });
    usvr->Init();
    usvr->Start();
    return 0;
}