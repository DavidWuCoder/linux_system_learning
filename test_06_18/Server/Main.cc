#include <iostream>
#include <memory>

#include "Common.hpp"
#include "Connection.hpp"
// clang-format off
#include "Log.hpp"
#include "Reactor.hpp"
#include "Listener.hpp"
#include "Protocol.hpp"
#include "NetCal.hpp"
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

    // 1. 构建一个业务模块
    std::shared_ptr<Cal> cal = std::make_shared<Cal>();

    // 2. 构建协议对象
    std::shared_ptr<Protocol> protocol = std::make_shared<Protocol>(
        [&cal](Request &req) -> Response { return cal->Execute(req); });

    // 3. 构建listener对象
    std::shared_ptr<Connection> conn = std::make_shared<Listener>(port);
    conn->RegisterHandler(
        [&protocol](std::string &inbuffer) -> std::string
        {
            LOG(LogLevel::DEBUG) << "进入匿名函数中";
            std::string response_str;
            while (true)
            {
                std::string package;
                if (!protocol->Decode(inbuffer, &package))
                {
                    break;
                }
                response_str += protocol->Excute(package);
            }
            LOG(LogLevel::DEBUG) << "结束匿名函数: " << response_str;
            return response_str;
        });

    std::unique_ptr<Reactor> R = std::make_unique<Reactor>();

    R->AddConnection(conn);
    R->Loop();

    return 0;
}
