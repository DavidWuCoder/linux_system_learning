#pragma once

#include <vector>
#include "Sem.hpp"
#include "Mutex.hpp"

using namespace SemModule;
using namespace MutexModule;

const int gcap = 5;

template <class T>
class RingQueue
{
public:
    RingQueue(int cap = gcap)
        : _cap(cap), _rq(cap), _blank_sem(cap), _data_sem(0), _p_step(0), _c_step(0)
    {
    }

    // 先申请信号量再加锁，还是先加锁再申请信号量？
    // 先申请信号量，再加锁！！！ 保证效率
    // 线程先瓜分信号量，多个线程再排队等待锁
    void Equeue(const T &in)
    {
        // 生产者
        // 1.申请信号量，空位置信号量
        _blank_sem.P();
        {
            LockGuard lockguard(_p_mutex);
            // 2.生产
            _rq[_p_step] = in;
            // 3.更新下标
            ++_p_step;
            // 4.维持环形特性
            _p_step %= _cap;
        }
        _data_sem.V();
    }

    void Pop(T *out)
    {
        // 消费者
        // 1. 申请信号量，数据信号量
        _data_sem.P();
        {
            LockGuard lockguard(_c_mutex);
            // 2. 消费
            *out = _rq[_c_step];
            // 3.更新下标;
            ++_c_step;
            // 4.维持环状特性
            _c_step %= _cap;
        }
        _blank_sem.V();
    }

    ~RingQueue()
    {
    }

private:
    std::vector<T> _rq;
    int _cap;

    // 生产者
    Sem _blank_sem;
    int _p_step;

    // 消费者
    Sem _data_sem;
    int _c_step;

    // 维护多生产、多消费，两把锁
    Mutex _c_mutex;
    Mutex _p_mutex;
};