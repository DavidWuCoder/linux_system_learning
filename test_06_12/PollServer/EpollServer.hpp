#include <sys/epoll.h>
#include <sys/types.h>

#include "Log.hpp"
#include "Socket.hpp"

using namespace SocketModule;

class EpollServer
{
    const static int size = 64;
    const static int defaultfd = -1;

public:
    EpollServer(int port)
        : _listensock(std::make_unique<TcpSocket>()),
          _isrunning(false),
          _epfd(defaultfd)
    {
        // 创建listensock套接字
        _listensock->BuildListenSocketMethod(port);

        // 2.创建并初始化epoll模型
        _epfd = epoll_create(256);

        if (_epfd < 0)
        {
            LOG(LogLevel::FATAL) << "epoll_create error";
        }
        LOG(LogLevel::INFO) << "epoll_create success";

        // 3.将listensock设置到内核中
        struct epoll_event ev;
        ev.events = EPOLLIN;
        ev.data.fd = _listensock->Fd();

        int n = epoll_ctl(_epfd, EPOLL_CTL_ADD, ev.data.fd, &ev);

        if (n < 0)
        {
            LOG(LogLevel::FATAL) << "epoll_ctl error";
        }
    }

    void Start()
    {
        _isrunning = true;
        while (_isrunning)
        {
            int n = epoll_wait(_epfd, _recvs, size, -1);
            switch (n)
            {
                case 0:
                    LOG(LogLevel::DEBUG) << "time out...";
                    break;
                case -1:
                    LOG(LogLevel::FATAL) << "epoll_wait error";
                    break;
                default:
                    Dispatcher(n);
                    break;
            }
        }
        _isrunning = false;
    }

    // 事件派发器
    void Dispatcher(int rnum)
    {
        LOG(LogLevel::DEBUG) << "event ready...";
        for (int i = 0; i < rnum; i++)
        {
            int sockfd = _recvs[i].data.fd;
            uint32_t revents = _recvs[i].events;
            if (revents & EPOLLIN)
            {
                // 写事件就绪
                if (sockfd == _listensock->Fd())
                {
                    // 新连接到来
                    Accepter();
                }
                else
                {
                    // 普通读事件
                    Recver(sockfd);
                }
            }
        }
    }
    //
    // 连接处理器
    void Accepter()
    {
        InetAddr client;
        int sockfd = _listensock->Accept(&client);
        // 将sockfd填入表中
        if (sockfd >= 0)
        {
            struct epoll_event ev;
            ev.events = EPOLLIN;
            ev.data.fd = sockfd;

            int n = epoll_ctl(_epfd, EPOLL_CTL_ADD, sockfd, &ev);
            if (n < 0)
            {
                LOG(LogLevel::DEBUG) << "epoll_ctl_add failed";
            }
            else
            {
                LOG(LogLevel::DEBUG) << "epoll_ctl_add success";
            }
        }
    }
    //
    // // IO处理器
    void Recver(int sockfd)
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
            // 从epoll 中删除
            int n = epoll_ctl(_epfd, EPOLL_CTL_DEL, sockfd, nullptr);
            if (n == 0)
            {
                LOG(LogLevel::DEBUG)
                    << "epoll_ctl_del success, sockfd: " << sockfd;
            }
            close(sockfd);
        }
        else
        {
            // 异常了
            // 客户端退出，select不再关注该文件描述符
            LOG(LogLevel::ERROR) << "recv_error";
            int n = epoll_ctl(_epfd, EPOLL_CTL_DEL, sockfd, nullptr);
            if (n == 0)
            {
                LOG(LogLevel::DEBUG)
                    << "epoll_ctl_del success, sockfd: " << sockfd;
            }
            close(sockfd);
        }
    }

    ~EpollServer()
    {
        _listensock->Close();
        if (_epfd > 0)
        {
            close(_epfd);
        }
    }

private:
    std::unique_ptr<TcpSocket> _listensock;
    bool _isrunning;

    int _epfd;
    struct epoll_event _recvs[size];
};
