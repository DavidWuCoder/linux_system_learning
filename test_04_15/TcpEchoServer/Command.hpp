#pragma once
#include <set>
#include <cstdio>
#include "Common.hpp"
#include "inetAddr.hpp"
#include "Log.hpp"

using namespace LogModule;

class Command
{
public:
    Command() 
    {
        // 严格匹配
        _whiteListCommands.insert("ls");
        _whiteListCommands.insert("pwd");
        _whiteListCommands.insert("ls -l");
        _whiteListCommands.insert("ll");
        _whiteListCommands.insert("touch haha.txt");
        _whiteListCommands.insert("who");
        _whiteListCommands.insert("whoami");
    }

    bool IsSafeCoammd(const std::string& cmd)
    {
        auto iter = _whiteListCommands.find(cmd);
        return iter != _whiteListCommands.end();
    }

    std::string Excute(const std::string cmd, InetAddr & addr)
    {
        // 1.检查命令是否属于白名单
        if (!IsSafeCoammd(cmd))
        {
            return std::string("该命令无法执行");
        }

        std::string who = addr.StringAddr();

        // 2.执行命令
        FILE* fp = popen(cmd.c_str(), "r");
        if (nullptr == fp)
        {
            return std::string("你要执行的命令不存在") + cmd;
        }
        std::string res;
        char line[1024];
        while (fgets(line, sizeof(line), fp))
        {
            res += line;
        }
        pclose(fp);
        std::string result = who + "excute done, result: \n" + res;
        LOG(LogLevel::DEBUG) << result;
        return result;
    }

    ~Command() {}
private:
    // 受限制的远程执行
    std::set<std::string> _whiteListCommands;
};