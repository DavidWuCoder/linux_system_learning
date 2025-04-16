#pragma once

#include "Common.hpp"
#include "Log.hpp"
#include "inetAddr.hpp"

namespace SocketModule
{
    using namespace LogModule;
    const static int gbacklog = 16;

    class Socket
    {
    public:
        virtual  ~Socket()
        {}

        virtual void SocketOrDie() = 0;
        virtual void BindOrDie(uint16_t port) = 0;
        virtual void ListenOrDie(int backlog) = 0;
        virtual std::shared_ptr<Socket> Accept(InetAddr *client) = 0;
        virtual void Close() = 0;
    public:
        void BuildListenSocketMethod(uint16_t port, int backlog = gbacklog)
        {
            SocketOrDie();
            BindOrDie(port);
            ListenOrDie(backlog);
        }

        // void BuildUdpSocketMethod()
        // {
        //     SocketOrDie();
        //     BindOrDie();
        // }
    };

    const static int  defaultfd = -1;
    class TcpSocket : public Socket
    {
    public:
        TcpSocket()
        : _sockfd(defaultfd)
        {}

        TcpSocket(int fd)
        :_sockfd(fd)
        {}

        ~TcpSocket()
        {}

        void SocketOrDie() override
        {
            _sockfd = ::socket(AF_INET, SOCK_STREAM, 0);
            if (_sockfd < 0)
            {
                LOG(LogLevel::FATAL) << "socket error";
                exit(SOCKET_ERR);
            }
            LOG(LogLevel::INFO) << "socket success, sockfd: " << _sockfd;
        }
        void BindOrDie(uint16_t port) override
        {
            InetAddr localAddr(port);
            int n = ::bind(_sockfd, localAddr.NetAddrPtr(), localAddr.NetAddrLen());
            if (n < 0)
            {
                LOG(LogLevel::FATAL) << "bind error";
                exit(BIND_ERR);
            }
            LOG(LogLevel::INFO) << "bind success";
        }
        void ListenOrDie(int backlog) override
        {
            int n = ::listen(_sockfd, backlog);
            if (n < 0)
            {
                LOG(LogLevel::FATAL) << "listen error";
                exit(LISTEN_ERR);
            }
            LOG(LogLevel::INFO) << "listen success";
        }
        std::shared_ptr<Socket> Accept(InetAddr *client) override
        {
            struct sockaddr_in peer;
            socklen_t len = sizeof(peer);
            int fd = accept(_sockfd, CONV(peer), &len);
            if (fd < 0)
            {
                LOG(LogLevel::WARNING) << "accept warning ...";
                return nullptr;
            }
            client->SetAddr(peer);
            return std::make_shared<TcpSocket>(fd);
        }

        void Close()
        {
            if (_sockfd > 0)
            {
                ::close(_sockfd);
            }
        }

    private:
        int _sockfd;
    };
    
    // class UdpSocket : public Socket
    // {
    // public:
    //     ~TcpSocket() {}

    // }
}