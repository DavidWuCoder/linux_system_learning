#include "BlockQueue.hpp"

void *consumer(void *args)
{
    BlockQueue<int> *bq = static_cast<BlockQueue<int> *>(args);
    while (true)
    {
        int data = bq->Pop();
        std::cout << "消费了一个数据：" << data << std::endl;
        sleep(1);
    }
}

void *producer(void *args)
{
    int data = 1;
    BlockQueue<int> *bq = static_cast<BlockQueue<int> *>(args);
    while (true)
    {
        std::cout << "生产了一个数据：" <<data << std::endl;
        bq->Equeue(data);
        data++;
        // sleep(1);
    }
}

int main()
{
    // 申请阻塞队列
    BlockQueue<int> *bq = new BlockQueue<int>();

    // 构建生产者消费者
    pthread_t c, p;

    pthread_create(&c, nullptr, consumer, bq);
    pthread_create(&p, nullptr, producer, bq);

    pthread_join(c, nullptr);
    pthread_join(p, nullptr);
    return 0;
}