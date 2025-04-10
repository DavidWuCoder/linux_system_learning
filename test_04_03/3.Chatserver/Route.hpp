#pragma once

#include <iostream>
#include <string>
#include <vector>
#include "inetAddr.hpp"
#include "Log.hpp"

using namespace LogModule;

class Route
{
private:
    bool IsExist(InetAddr &peer)
    {
        for (auto &user : _online_user)
        {
            if (user == peer)
            {
                return false;
            }
        }
        return true;
    }

    void AddUser(InetAddr &peer)
    {
        LOG(LogLevel::DEBUG) << "新增一个在线用户" << peer.StringAddr();
        _online_user.push_back(peer);
    }

    void DeleteUser(InetAddr &peer)
    {
        for (auto iter = _online_user.begin(); iter != _online_user.end(); iter++)
        {
            LOG(LogLevel::DEBUG) << "删除用户成功: " << peer.StringAddr();
            _online_user.erase(iter);
            break;
        }
    }

public:
    Route()
    {
    }

    void MessageRoute(int sockfd, const std::string &message, InetAddr &peer)
    {
        if (!IsExist(peer))
        {
            AddUser(peer);
        }

        std::string send_message = peer.StringAddr() + "#" + message;
        for (auto &user : _online_user)
        {
            sendto(sockfd, send_message.c_str(), send_message.size(), 0, (const struct sockaddr *)&user.NetAddr(), sizeof(user.NetAddr()));
        }

        // 这个用户一定在线了
        if (message == "QUIT")
        {
            LOG(LogLevel::DEBUG) << "用户退出： " << peer.StringAddr();
            DeleteUser(peer);
        }
    }

    ~Route()
    {
    }

private:
    // 首次发消息视为登录
    std::vector<InetAddr> _online_user; // 在线用户
};