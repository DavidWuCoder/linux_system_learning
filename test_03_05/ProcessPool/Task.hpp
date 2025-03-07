#pragma once

#include <iostream>
#include <vector>
#include <ctime>

typedef void (*task_t)();

void PrintLog()
{
    std::cout << "我是一个打印日志的任务" << std::endl;
}

void Download()
{
    std::cout << "我是一个下载任务" << std::endl;
}

void Upload()
{
    std::cout << "我是一个上传任务" << std::endl;
}

class TaskManager
{
public:
    TaskManager()
    {
        srand(time(nullptr));
    }
    void Register(task_t t)
    {
        _tasks.push_back(t);
    }
    int Code()
    {
        return rand() % _tasks.size();
    }
    void Excute(int code)
    {
        if (code >= 0 && code <= _tasks.size())
        {
            _tasks[code]();
        }
    }
    ~TaskManager()
    {
    }

private:
    std::vector<task_t> _tasks;
};