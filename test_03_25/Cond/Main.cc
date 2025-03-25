#include "BlockQueue.hpp"
#include "Task.hpp"
#include <iostream>
#include <pthread.h>
#include <unistd.h>

void *consumer(void *args)
{
    BlockQueue<task_t> *bq = static_cast<BlockQueue<task_t> *>(args);

    while (true)
    {
        sleep(10);
        // 1. 消费任务
        task_t t = bq->Pop();

        // 2. 处理任务 -- 处理任务的时候，这个任务，已经被拿到线程的上下文中了,不属于队列了
        t();
    }
}

void *productor(void *args)
{
    BlockQueue<task_t> *bq = static_cast<BlockQueue<task_t> *>(args);
    while (true)
    {
        // 1. 获得任务
        //std::cout << "生产了一个任务: " << x << "+" << y << "=?" << std::endl;
        std::cout << "生产了一个任务: " << std::endl;

        // 2. 生产任务
        bq->Equeue(Download);
    }
}

int main()
{
    // 扩展认识: 阻塞队列: 可以放任务吗？
    // 申请阻塞队列
    BlockQueue<task_t> *bq = new BlockQueue<task_t>();

    // 构建生产和消费者
    pthread_t c[0], p[0];

    pthread_create(c, nullptr, consumer, bq);
    // pthread_create(c+1, nullptr, consumer, bq);
    pthread_create(p, nullptr, productor, bq);
    // pthread_create(p+1, nullptr, productor, bq);
    // pthread_create(p+2, nullptr, productor, bq);

    pthread_join(c[0], nullptr);
    // pthread_join(c[1], nullptr);
    pthread_join(p[0], nullptr);
    // pthread_join(p[1], nullptr);
    // pthread_join(p[2], nullptr);

    return 0;
}















// #include "BlockQueue.hpp"
// #include "Task.hpp"
// #include <iostream>
// #include <pthread.h>
// #include <unistd.h>

// void *consumer(void *args)
// {
//     BlockQueue<Task> *bq = static_cast<BlockQueue<Task> *>(args);

//     while (true)
//     {
//         sleep(1);
//         Task t = bq->Pop();

//         t.Execute();

//         std::cout << "消费了一个任务: " << t.X() << "+" << t.Y() << "=" << t.Result()  << std::endl;
//     }
// }

// void *productor(void *args)
// {
//     int x = 1;
//     int y = 1;
//     BlockQueue<Task> *bq = static_cast<BlockQueue<Task> *>(args);
//     while (true)
//     {
//         // sleep(1);
//         std::cout << "生产了一个任务: " << x << "+" << y << "=?" << std::endl;
//         Task t(x, y);
//         bq->Equeue(t);

//         x++, y++;
//     }
// }

// int main()
// {
//     // 扩展认识: 阻塞队列: 可以放任务吗？
//     // 申请阻塞队列
//     BlockQueue<Task> *bq = new BlockQueue<Task>();

//     // 构建生产和消费者
//     pthread_t c, p;

//     pthread_create(&c, nullptr, consumer, bq);
//     pthread_create(&p, nullptr, productor, bq);

//     pthread_join(c, nullptr);
//     pthread_join(p, nullptr);

//     return 0;
// }