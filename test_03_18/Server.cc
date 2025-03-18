#include "MsgQueue.hpp"
#include "ChainOfResponsibility.hpp"

int main()
{
    Server server;
    std::string text;

    HandlerEntry he;
    he.EnableHandler(true, true, true);

    while (true)
    {
        // 消息队列为空，则阻塞等待
        server.Recv(MSG_TYPE_CLIENT, text);

        std::cout << "Received: " << text << std::endl;
        if (text == "exit")
        {
            std::cout << "break!!" << std::endl;
            break;
        }
        he.Run(text);

    }
    return 0;
}