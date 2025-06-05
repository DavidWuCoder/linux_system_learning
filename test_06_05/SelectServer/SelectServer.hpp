#include <sys/select.h>

#include "Log.hpp"
#include "Socket.hpp"

using namespace SocketModule;

class SelectServer
{
    int size = 1024;
    int defaultfd = -1;

public:
    SelectServer(int port)
        : _listensock(std::make_unique<TcpSocket>()), _isrunning(false)
    {
        _listensock->BuildListenSocketMethod(port);

        for (int i = 0; i < size; i++)
        {
            _fd_arr[i] = -1;
        }

        _fd_arr[0] = _listensock->Fd();
    }

    void Start()
    {
        _isrunning = true;
        while (_isrunning)
        {
            // 每次都需要对数组进行处理
            fd_set rfds;
            FD_ZERO(&rfds);
            int maxfd = _listensock->Fd();
            for (int i = 0; i < size; i++)
            {
                if (_fd_arr[i] == defaultfd) continue;
                FD_SET(_fd_arr[i], &rfds);
                if (_fd_arr[i] > maxfd)
                {
                    maxfd = _fd_arr[i];
                }
            }
            Printfd();

            // select负责事件就绪检测
            int n = select(maxfd + 1, &rfds, nullptr, nullptr, nullptr);
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
                    Dispatcher(rfds);  // 事件派发函数
                    break;
            }
        }
        _isrunning = false;
    }

    // 事件派发器
    void Dispatcher(fd_set& rfds)
    {
        // 可能是建立连接的事件，也有可能是读事件就绪

        for (int i = 0; i < size; i++)
        {
            if (_fd_arr[i] == defaultfd)
            {
                continue;
            }

            if (FD_ISSET(_fd_arr[i], &rfds))
            {
                // 可能是建立连接的事件，也有可能是读事件就绪

                if (_fd_arr[i] == _listensock->Fd())
                {
                    LOG(LogLevel::INFO) << "连接事件就绪";
                    // 处理连接
                    Accepter(rfds);
                }
                else
                {
                    // 普通读事件就绪了

                    LOG(LogLevel::INFO) << "读事件就绪";
                    Recver(_fd_arr[i], i);
                }
            }
        }
    }

    // 连接处理器
    void Accepter(fd_set& rfds)
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
                if (_fd_arr[pos] == defaultfd)
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
                _fd_arr[pos] = sockfd;
            }
        }
    }

    // IO处理器
    void Recver(int sockfd, int pos)
    {
        char buffer[1024];
        ssize_t n = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
        if (n > 0)
        {
            buffer[n - 1] = 0;
            std::cout << "client say@" << buffer << std::endl;
        }
        else if (n == 0)
        {
            LOG(LogLevel::INFO) << "client quit";
            // 客户端退出，select不再关注该文件描述符
            _fd_arr[pos] = defaultfd;
            close(sockfd);
        }
        else
        {
            // 异常了
            // 客户端退出，select不再关注该文件描述符
            LOG(LogLevel::ERROR) << "recv_error";
            _fd_arr[pos] = defaultfd;
            close(sockfd);
        }
    }

    void Printfd()
    {
        std::cout << "---------------------------------" << std::endl;
        for (int i = 0; i < size; i++)
        {
            if (_fd_arr[i] != defaultfd)
            {
                std::cout << _fd_arr[i] << ' ';
            }
        }
        std::cout << std::endl;
        std::cout << "---------------------------------" << std::endl;
    }

    ~SelectServer() {}

private:
    std::unique_ptr<TcpSocket> _listensock;
    bool _isrunning;
    int _fd_arr[1024];
};
