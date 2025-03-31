#ifndef __LOG_HPP__
#define __LOG_HPP__

#include <cstdio>
#include <iostream>
#include <filesystem> // 需要c++17
#include <fstream>
#include <memory>
#include <sstream>
#include <ctime>
#include <unistd.h>
#include "Mutex.hpp"

namespace LogModule
{
    using namespace MutexModule;
    const std::string gsep = "\r\n";

    // 刷新策略（显示器打印，指定文件打印）
    // 策略模式， 本质就是多态特性
    class LogStrategy
    {
    public:
        ~LogStrategy() = default;
        virtual void SyncLog(const std::string &message) = 0;
    };

    // 显示器打印日志策略 子类
    class ConsoleLogStrategy : public LogStrategy
    {
    public:
        ConsoleLogStrategy() {}

        void SyncLog(const std::string &message) override
        {
            LockGuard lockguard(_mutex);
            std::cout << message << gsep;
        }

        ~ConsoleLogStrategy() {}

    private:
        Mutex _mutex;
    };

    // 文件打印策略 子类
    const std::string defaultpath = "./log";
    const std::string defaultfile = "my.log";
    class FileLogStrategy : public LogStrategy
    {
    public:
        FileLogStrategy(const std::string &path = defaultpath,
                        const std::string &file = defaultfile)
            : _path(path), _file(file)
        {
            LockGuard lockguard(_mutex);
            // 路径构建
            if (std::filesystem::exists(_path))
            {
                return;
            }
            try
            {
                std::filesystem::create_directories(_path);
            }
            catch (const std::filesystem::filesystem_error &e)
            {
                std::cerr << e.what() << '\n';
            }
        }

        void SyncLog(const std::string &message) override
        {
            LockGuard lockguard(_mutex);

            std::string filename = _path + (_path.back() != '/' ? "/" : "") + _file; // "./log" + "my.log"
            std::ofstream out(filename, std::ios::app);
            if (!out.is_open())
            {
                std::cerr << "file open error" << std::endl;
                return;
            }

            out << message << gsep;

            out.close();
        }

        ~FileLogStrategy()
        {
        }

    private:
        Mutex _mutex;
        std::string _path; // 日志文件所在路径
        std::string _file; // 日志文件本身
    };

    // 形成一条完成的日志

    // 1.等级
    enum class LogLevel
    {
        DEBUG,
        INFO,
        WARNING,
        ERROR,
        FATAL
    };

    std::string Level2Str(LogLevel level)
    {
        switch (level)
        {
        case LogLevel::DEBUG:
            return "DEBUG";
        case LogLevel::INFO:
            return "INFO";
        case LogLevel::WARNING:
            return "WARNING";
        case LogLevel::ERROR:
            return "ERROR";
        case LogLevel::FATAL:
            return "FATAL";
        default:
            return "UNKNOWN";
        }
    }

    // 2.获取时间功能
    std::string GetTimeStamp()
    {
        time_t curr = time(nullptr);
        struct tm curr_tm;
        localtime_r(&curr, &curr_tm);
        char timebuffer[128];
        snprintf(timebuffer, sizeof(timebuffer), "%4d-%02d-%02d %02d:%02d:%02d",
            curr_tm.tm_year + 1900, curr_tm.tm_mon + 1, curr_tm.tm_mday,
             curr_tm.tm_hour, curr_tm.tm_min, curr_tm.tm_sec
        );
        return timebuffer;
    }

    // 日志类 （按照需求形成日志， 根据不同策略完成刷新）
    class Logger
    {
    public:
        Logger()
        {
            EnableConsoleLogStrategy();
        }

        void EnableFileLogStrategy()
        {
            _fflush_strategy = std::make_unique<FileLogStrategy>(); // c++ 14
        }

        void EnableConsoleLogStrategy()
        {
            _fflush_strategy = std::make_unique<ConsoleLogStrategy>();
        }

        // 表示一条完成的日志
        class LogMessage
        {
        public:
            LogMessage(LogLevel &level, std::string &src_name, int _line_number, Logger &logger)
                : _curr_time(GetTimeStamp()),
                 _level(level),
                  _src_name(src_name),
                  _pid(getpid()),
                 _line_number(_line_number),
                 _logger(logger)
            {
                // 日志的前缀
                std::stringstream ss;
                ss << "[" << _curr_time << "]"
                   << "[" << Level2Str(_level) << "]"
                   << "[" << _pid << "]"
                   << "[" << _src_name << "]"
                   << "[" << _line_number << "]"
                   << "-";
                _loginfo = ss.str();
            }
            // 需要支持 "<<"的重载

            template <class T>
            LogMessage &operator<<(const T &info)
            {
                // 日志的消息，可变的
                std::stringstream ss;
                ss << info;
                _loginfo += ss.str();
                return *this;
            }

            ~LogMessage()
            {
                if (_logger._fflush_strategy)
                {
                    _logger._fflush_strategy->SyncLog(_loginfo);
                }
            }

        private:
            std::string _curr_time; // 当前时间
            LogLevel _level;        // 等级
            pid_t _pid;             // 进程pid
            std::string _src_name;  // 文件名
            int _line_number;       // 行号
            std::string _loginfo;   // 合并后的完整信息
            Logger &_logger;
        };

        LogMessage operator()(LogLevel level, std::string name, int line)
        {
            return LogMessage(level, name, line, *this);
        }

        ~Logger() {}

    private:
        std::unique_ptr<LogStrategy> _fflush_strategy;
    };

    // 全局日志对象
    Logger logger;

    // 使用宏来简化用户操作，并且获取文件名和行号
    #define LOG(level) logger(level, __FILE__, __LINE__)

    #define Enable_Console_Log_Strategy() logger.EnableConsoleLogStrategy()
    #define Enable_File_Log_Strategy() logger.EnableFileLogStrategy()

}

#endif