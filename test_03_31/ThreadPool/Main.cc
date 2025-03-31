#include "Log.hpp"
#include <memory>

using namespace LogModule;

int main()
{
    // std::unique_ptr<LogStrategy> strategy = std::make_unique<ConsoleLogStrategy>();
    // std::unique_ptr<LogStrategy> strategy = std::make_unique<FileLogStrategy>();
    // strategy->SyncLog("hello world");

    // logger.EnableFileLogStrategy();
    // logger(LogLevel::DEBUG, "main.cc", 10) << "hello world" << 42398 << 4.1123;
    // logger(LogLevel::DEBUG, "main.cc", 10) << "hello world";
    // logger(LogLevel::DEBUG, "main.cc", 10) << "hello world";
    // logger(LogLevel::DEBUG, "main.cc", 10) << "hello world";

    Enable_Console_Log_Strategy();
    LOG(LogLevel::DEBUG) << "hello world" << 3.14 ;
    return 0;
}
