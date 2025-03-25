#pragma once
#include <functional>
#include <unistd.h>

// 任务类型2
using task_t = std::function<void()>;

void Download()
{
    std::cout << "我是一个下载任务..." << std::endl;
    sleep(3); // 假设处理任务比较耗时
}


// 任务类型1
class Task
{
public:
    Task() {};
    Task(int x, int y) : _x(x), _y(y)
    {}

    int X() {return _x;}
    int Y() {return _y;}   
    int Result(){return _result;}


    void Excute()
    {
        _result = _x + _y;
    }

private:
    int _x;
    int _y;
    int _result;

};