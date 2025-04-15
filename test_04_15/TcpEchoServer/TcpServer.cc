#include "TcpServer.hpp"
#include "Dict.hpp"
#include "Command.hpp"

std::string defaulthandler(const std::string &word)
{
    std::string s = "haha, ";
    s += word;
    return s;
}

void Usage(std::string proc)
{
    std::cerr << "Usage: " << proc << " port" << std::endl;
}

// ./tcpserver port
int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        Usage(argv[0]);
        exit(USAGE_ERR);
    }
    uint16_t port = std::stoi(argv[1]);

    Enable_Console_Log_Strategy();

    // Dict d;
    // d.LoadDict();
    // std::unique_ptr<TcpServer> tsvr = std::make_unique<TcpServer>(port, [&d](const std::string &word, InetAddr &addr){
    //     return d.Translate(word, addr);
    // });

    Command cmd;

    func_t f = std::bind(&Command::Excute, &cmd, std::placeholders::_1, std::placeholders::_2);

    std::unique_ptr<TcpServer> tsvr = std::make_unique<TcpServer>(port, f);
    tsvr->Init();
    tsvr->Run();

    return 0;
}