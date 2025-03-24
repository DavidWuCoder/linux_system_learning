#pragma once

#include <iostream>
#include <string>
#include <queue>
#include <pthread.h>
#include <unistd.h>

const int defaultcap = 5;

template <class T>
class BlockQueue
{
    bool Isfull() {return _q.size() >= _cap;}
    bool Isempty() {return _q.size() <= 0;}
public:
    BlockQueue(int cap = defaultcap)
        : _cap(cap), _csleep_num(0), _psleep_num(0)
    {
        pthread_mutex_init(&_mutex, nullptr);
        pthread_cond_init(&_full_cond, nullptr);
        pthread_cond_init(&_empty_cond, nullptr);
    }

    void Equeue(const T &in)
    {
        pthread_mutex_lock(&_mutex);
        // 生产者调用
        if (Isfull())
        {
            // 等待
            _psleep_num++;
            pthread_cond_wait(&_full_cond, &_mutex);
            _psleep_num--;
        }
        _q.push(in);

        // 临时方案
        if (_csleep_num > 0)
        {
            pthread_cond_signal(&_empty_cond);
            std::cout << "唤醒消费者" << std::endl;
        }
        pthread_mutex_unlock(&_mutex);
    }

    T Pop()
    {
        // 消费者调用
        pthread_mutex_lock(&_mutex);
        if (Isempty())
        {
            _csleep_num++;
            pthread_cond_wait(&_empty_cond, &_mutex);
            _csleep_num--;
        }
        T data = _q.front();
        _q.pop();

        // 消费者唤醒生产者
        if (_psleep_num > 0)
        {
            pthread_cond_signal(&_full_cond);     
            std::cout << "唤醒生产者" << std::endl;
        }
        pthread_mutex_unlock(&_mutex);
        return data;
    }

    ~BlockQueue()
    {
        pthread_mutex_destroy(&_mutex);
        pthread_cond_destroy(&_full_cond);
        pthread_cond_destroy(&_empty_cond);
    }

private:
    std::queue<T> _q; // 阻塞队列
    int _cap;         // 容量
    pthread_mutex_t _mutex;
    pthread_cond_t _full_cond;  // 队列满的条件变量
    pthread_cond_t _empty_cond; // 队列空的条件变量

    int _csleep_num;
    int _psleep_num;
};