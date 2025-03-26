#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include "RingQueue.hpp"

struct threaddata
{
    RingQueue<int>* rq;
    std::string _name;
};

void *consumer(void *args)
{
    threaddata* td = static_cast<threaddata*>(args);

    while (true)
    {
        sleep(3);
        // 1. 消费任务
        int t = 0;
        td->rq->Pop(&t);
        std::cout << td->_name << " : " << "消费者拿到了一个数据" << t << std::endl;
    }
}

void *productor(void *args)
{
    threaddata* td = static_cast<threaddata*>(args);
    int data = 1;
    while (true)
    {
        sleep(1);
        // 1. 获得任务
        //std::cout << "生产了一个任务: " << x << "+" << y << "=?" << std::endl;
        std::cout <<td->_name << " : "<<"生产了一个任务: " << data << std::endl;

        // 2. 生产任务
        td->rq->Equeue(data++);
    }
}

int main()
{
    // 扩展认识: 阻塞队列: 可以放任务吗？
    // 申请阻塞队列
    RingQueue<int> *rq = new RingQueue<int>();

    // 构建生产和消费者
    pthread_t c[2], p[3];

    threaddata* td0 = new threaddata();
    td0->_name = "cthread-0";
    td0->rq = rq;

    pthread_create(c, nullptr, consumer, td0);

    threaddata* td1 = new threaddata();
    td1->_name = "cthread-1";
    td1->rq = rq;
    pthread_create(c + 1, nullptr, consumer, td1);

    threaddata* td2 = new threaddata();
    td2->_name = "pthread-2";
    td2->rq = rq;
    pthread_create(p, nullptr, productor, td2);

    threaddata* td3 = new threaddata();
    td3->_name = "pthread-3";
    td3->rq = rq;
    pthread_create(p + 1, nullptr, productor, td3);

    threaddata* td4 = new threaddata();
    td4->_name = "pthread-4";
    td4->rq = rq;
    pthread_create(p + 2, nullptr, productor, td4);

    pthread_join(c[0], nullptr);
    pthread_join(c[1], nullptr);
    pthread_join(p[0], nullptr);
    pthread_join(p[1], nullptr);
    pthread_join(p[2], nullptr);


    return 0;
}
