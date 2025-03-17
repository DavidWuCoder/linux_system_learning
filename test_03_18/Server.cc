#include "MsgQueue.hpp"

int main()
{
    Server server;
    std::string text;
    while (true)
    {
        // 消息队列为空，则阻塞等待
        server.Recv(MSG_TYPE_CLIENT, text);
        std::cout << "Received: " << text << std::endl;
        if (text == "exit")
        {
            break;
        }
    }
    return 0;
}