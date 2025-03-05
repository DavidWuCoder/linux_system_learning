#include "ProcessPool.hpp"

int main()
{
    // 创建进程池对象
    ProcessPool pp(gdefaultnum);

    // 启动进程池
    pp.Start();

    // 自动派发任务
    int cnt = 10;
    while (cnt--)
    {
        // 1.选择一个信道
        pp.Run();
        sleep(1);
    }
    pp.Stop();
    sleep(1000);
    return 0;
}