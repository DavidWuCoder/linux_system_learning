#include "Socket.hpp"
#include <iostream>
#include <memory>
#include <sys/wait.h>
#include <functional>

using namespace SocketModule;
using namespace LogModule;

using ioservice_t = std::function<void(std::shared_ptr<Socket>& sock, InetAddr& client)>;

class TcpServer
{
public:
    TcpServer(uint16_t port, ioservice_t service)
        :_port(port)
        , _listensocketptr(std::make_unique<TcpSocket>())
        , _isrunning(false)
        , _service(service)
    {
        _listensocketptr->BuildListenSocketMethod(_port);
    }

    void Start()
    {
        _isrunning = true;

        while (true)
        {
            InetAddr client;
            auto sock = _listensocketptr->Accept(&client);
            if (sock == nullptr)
            {
                continue;
            }
            LOG(LogLevel::DEBUG) << "accept success" << client.StringAddr();

            // sock && client
            pid_t id = fork();
            if (id < 0)
            {
                LOG(LogLevel::FATAL) << "fork error";
                exit(FORK_ERR);
            }
            else if (id == 0)
            {
                // child
                _listensocketptr->Close();
                if (fork() > 0)
                {
                    exit(OK);
                }
                _service(sock, client);
                sock->Close();
                exit(OK);
            }
            else 
            {
                // father
                sock->Close();
                pid_t rid = waitpid(id, nullptr, 0);
            }
        }

        _isrunning = false;
    }

private:
    uint16_t _port;
    std::unique_ptr<Socket> _listensocketptr;
    bool _isrunning;
    ioservice_t _service;
};