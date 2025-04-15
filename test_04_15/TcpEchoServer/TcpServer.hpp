#pragma once
#include "Log.hpp"
#include "Common.hpp"
#include "inetAddr.hpp" // 注意包含的顺序
#include <sys/wait.h>
// #include <signal.h>

using namespace LogModule;

const static int defaultsockfd = -1;
const int backlog = 8;

class TcpServer : public NoCopy
{
public:
    TcpServer(uint16_t port) : _port(port), _listensockfd(defaultsockfd), _isrunning(false)
    {
    }

    void Init()
    {
        // signal(SIGCHLD, SIG_IGN); // 推荐的做法
        // 1.创建套接字文件
        _listensockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (_listensockfd < 0)
        {
            LOG(LogLevel::FATAL) << "socket error";
            exit(SOCKET_ERR);
        }
        LOG(LogLevel::INFO) << "socket success: " << _listensockfd; // 3

        // 2.bind 端口号
        InetAddr local(_port);
        int n = bind(_listensockfd, local.NetAddrPtr(), local.NetAddrLen());
        if (n < 0)
        {
            LOG(LogLevel::FATAL) << "bind error";
            exit(BIND_ERR);
        }
        LOG(LogLevel::INFO) << "bind success: " << _listensockfd; // 3

        // 3.设置
        n = listen(_listensockfd, backlog);
        if (n < 0)
        {
            LOG(LogLevel::FATAL) << "listen Error";
            exit(LISTEN_ERR);
        }
        LOG(LogLevel::INFO) << "listen success: " << _listensockfd; // 3
    }

    class ThreadData
    {
    public:
        ThreadData(int fd, InetAddr &ar, TcpServer *s)
            : _sockfd(fd),
              _addr(ar), tsvr(s)
        {
        }

    public:
        int _sockfd;
        InetAddr _addr;
        TcpServer *tsvr;
    };

    void Service(int sockfd, InetAddr &addr)
    {
        char buffer[1024];
        memset(buffer, 0, sizeof(buffer));
        while (true)
        {
            // 1.读取
            ssize_t n = read(sockfd, buffer, sizeof(buffer) - 1);
            if (n > 0)
            {
                buffer[n] = 0;
                LOG(LogLevel::DEBUG) << addr.StringAddr() << "say#" << buffer;

                // 2.写回
                std::string echo_string = "echo# ";
                echo_string += buffer;
                write(sockfd, echo_string.c_str(), echo_string.size());
            }
            else if (n == 0)
            {
                LOG(LogLevel::DEBUG) << addr.StringAddr() << "退出了...";
                close(sockfd);
                break;
            }
            else
            {
                LOG(LogLevel::DEBUG) << addr.StringAddr() << "异常...";
                close(sockfd);
                break;
            }
        }
    }

    static void *Routine(void *args)
    {
        pthread_detach(pthread_self());
        ThreadData *td = static_cast<ThreadData *>(args);
        td->tsvr->Service(td->_sockfd, td->_addr);
        delete td;
        return nullptr;
    }

    void Run()
    {
        _isrunning = true;

        while (true)
        {
            // a.获取链接
            struct sockaddr_in peer;
            socklen_t len = sizeof(sockaddr_in);
            // 没有连接就continue
            int sockfd = accept(_listensockfd, CONV(peer), &len);
            if (sockfd < 0)
            {
                LOG(LogLevel::WARNING) << "accept error";
                continue;
            }
            InetAddr addr(peer);
            LOG(LogLevel::INFO) << "accept success, peer addr: " << addr.StringAddr(); // 3
            // version0 --> test 单线程，仅测试
            // Service(sockfd, addr);

            // version1 多进程
            // pid_t id = fork();
            // if (id < 0)
            // {
            //     LOG(LogLevel::FATAL) << "fork error";
            //     exit(FORK_ERR);
            // }
            // else if (id == 0)
            // {
            //     // 子进程
            //     // 不希望子进程访问listensock
            //     close(_listensockfd);

            //     if (fork() > 0) // 再次fork
            //         exit(OK);

            //     Service(sockfd, addr); // 子进程的子进程
            //     exit(OK);
            // }
            // else
            // {
            //     // 父进程
            //     close(sockfd);

            //     pid_t rid = waitpid(id, nullptr, 0);
            //     (void)rid;
            // }

            // version2: 多线程版本
            ThreadData *td = new ThreadData(sockfd, addr, this);

            pthread_t tid;
            pthread_create(&tid, nullptr, Routine, td);
        }

        _isrunning = false;
    }

    ~TcpServer()
    {
    }

private:
    uint16_t _port;
    int _listensockfd; // 监听套接字
    bool _isrunning;
};