#include "Log.hpp"
#include "ThreadPool.hpp"
#include "Task.hpp"
#include <memory>

using namespace LogModule;
using namespace ThreadPoolModule;

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

    // Enable_Console_Log_Strategy();
    // LOG(LogLevel::DEBUG) << "hello world" << 3.14 ;

    // ThreadPool<task_t> *tp = new ThreadPool<task_t>();
    // tp->Start();
    
    int count = 10;

    while (true)
    {
        ThreadPool<task_t>::GetInstance()->Enqueue(Download);
        sleep(1);
        count--;
    }

    ThreadPool<task_t>::GetInstance()->Stop();

    ThreadPool<task_t>::GetInstance()->Join();
    return 0;
}
