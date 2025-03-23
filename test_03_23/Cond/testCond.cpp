#include <iostream>
#include <vector>
#include <pthread.h>
#include <string>
#include <unistd.h>

#define NUM 5

int cnt = 100;

pthread_mutex_t glock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t gcond = PTHREAD_COND_INITIALIZER;

// 等待的条件？比如票数为0,等待之前就要对资源的数量进行判定
// 判定本身就是在访问临界资源，判定语句（if）一定在临界区内部
// 判定结果，也一定是在临界资源内部。所以，条件变量不满足需要休眠，一定在临界区内休眠

void *routine(void *args)
{
    std::string name = static_cast<const char *>(args);
    while (true)
    {
        pthread_mutex_lock(&glock);
        // 使用条件变量让线程等待， 等待要在加锁解锁之间进行
        pthread_cond_wait(&gcond, &glock); // glock在wait之前会被自动释放掉
        std::cout << name << "计算：" << cnt << std::endl;
        cnt++;
        pthread_mutex_unlock(&glock);
    }
}

int main()
{
    std::vector<pthread_t> threads;
    for (int i = 0; i < NUM; i++)
    {
        pthread_t tid;
        char *name = new char[64];
        snprintf(name, 64, "thread-%d", i);
        int n = pthread_create(&tid, nullptr, routine, name);
        if (n != 0)
            continue;
        threads.push_back(tid);
        // sleep(1); // 加上这句能保证线程的进入等待的顺序符合编号0,1,2,3,4
    }

    sleep(3);

    // 每隔一秒唤醒一个线程
    while (true)
    {
        // std::cout << "唤醒所有线程" << std::endl;
        // pthread_cond_broadcast(&gcond);
        std::cout << "唤醒一个线程" << std::endl;
        pthread_cond_signal(&gcond);
        sleep(1);
    }

    for (auto& id : threads)
    {
        int m = pthread_join(id, nullptr);
        (void)m;
    }
    return 0;
}