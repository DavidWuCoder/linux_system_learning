#include <sys/poll.h>

#include "Log.hpp"
#include "Socket.hpp"

using namespace SocketModule;

class PollServer
{
    const static int size = 4096;
    const static int defaultfd = -1;

public:
    PollServer(int port)
        : _listensock(std::make_unique<TcpSocket>()), _isrunning(false)
    {
        _listensock->BuildListenSocketMethod(port);

        for (int i = 0; i < size; i++)
        {
            _fds[i].fd = defaultfd;
            _fds[i].events = 0;
            _fds[i].revents = 0;
        }
        _fds[0].fd = _listensock->Fd();
        _fds[0].events |= POLLIN;
    }

    void Start()
    {
        _isrunning = true;
        while (_isrunning)
        {
            Printfd();

            // Poll事件检测
            int n = poll(_fds, size, -1);
            switch (n)
            {
                case -1:
                    LOG(LogLevel::ERROR) << "select error";
                    break;
                case 0:
                    LOG(LogLevel::INFO) << "time out";
                    break;
                default:
                    LOG(LogLevel::INFO) << "有事件就绪";
                    Dispatcher();  // 事件派发函数
                    break;
            }
        }
        _isrunning = false;
    }

    // 事件派发器
    void Dispatcher()
    {
        // 可能是建立连接的事件，也有可能是读事件就绪

        for (int i = 0; i < size; i++)
        {
            if (_fds[i].fd == defaultfd)
            {
                continue;
            }

            if (_fds[i].revents & POLLIN)
            {
                // 可能是建立连接的事件，也有可能是读事件就绪

                if (_fds[i].fd == _listensock->Fd())
                {
                    LOG(LogLevel::INFO) << "连接事件就绪";
                    // 处理连接
                    Accepter();
                }
                else
                {
                    // 普通读事件就绪了

                    LOG(LogLevel::INFO) << "读事件就绪";
                    Recver(i);
                }
            }
        }
    }

    // 连接处理器
    void Accepter()
    {
        InetAddr client;
        int sockfd = _listensock->Accept(&client);
        // 将sockfd填入表中
        if (sockfd >= 0)
        {
            // 获取到新连接了
            LOG(LogLevel::INFO)
                << "获取到新连接，地址: " << client.StringAddr();

            int pos = 0;
            for (pos = 0; pos < size; pos++)
            {
                if (_fds[pos].fd == defaultfd)
                {
                    break;
                }
            }
            if (pos == size)
            {
                LOG(LogLevel::WARNING) << "server full";
                close(sockfd);
            }
            else
            {
                _fds[pos].fd = sockfd;
                _fds[pos].events |= POLLIN;
                _fds[pos].revents = 0;
            }
        }
    }

    // IO处理器
    void Recver(int pos)
    {
        char buffer[1024];
        ssize_t n = recv(_fds[pos].fd, buffer, sizeof(buffer) - 1, 0);
        if (n > 0)
        {
            buffer[n - 1] = 0;
            std::cout << "client say@" << buffer << std::endl;
        }
        else if (n == 0)
        {
            LOG(LogLevel::INFO) << "client quit";
            // 客户端退出，select不再关注该文件描述符
            close(_fds[pos].fd);
            _fds[pos].fd = defaultfd;
            _fds[pos].events = 0;
            _fds[pos].revents = 0;
        }
        else
        {
            // 异常了
            // 客户端退出，select不再关注该文件描述符
            LOG(LogLevel::ERROR) << "recv_error";
            close(_fds[pos].fd);
            _fds[pos].fd = defaultfd;
            _fds[pos].events = 0;
            _fds[pos].revents = 0;
        }
    }

    void Printfd()
    {
        std::cout << "---------------------------------" << std::endl;
        for (int i = 0; i < size; i++)
        {
            if (_fds[i].fd != defaultfd)
            {
                std::cout << _fds[i].fd << ' ';
            }
        }
        std::cout << std::endl;
        std::cout << "---------------------------------" << std::endl;
    }

    ~PollServer() {}

private:
    std::unique_ptr<TcpSocket> _listensock;
    bool _isrunning;

    struct pollfd _fds[size];
};
