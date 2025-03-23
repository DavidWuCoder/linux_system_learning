#ifndef _LOCK_HPP_
#define _LOCK_HPP_

#include <iostream>
#include <string>
#include <pthread.h>

namespace LockModule
{
    // 封装锁
    class Mutex
    {
    public:
        // 删除赋值和拷贝
        Mutex(const Mutex& ) = delete;
        const Mutex& operator=(const Mutex&) =delete;

        Mutex()
        {
            int n = pthread_mutex_init(&_mutex, nullptr);
            (void)n;
        }

        void Lock()
        {
            int n = pthread_mutex_lock(&_mutex);
            (void)n;
        }

        void Unlock()
        {
            int n = pthread_mutex_unlock(&_mutex);
            (void)n;
        }

        // 获取指针的接口
        pthread_mutex_t *GetMutexOriginal()
        {
            return &_mutex;
        }

        ~Mutex()
        {
            int n = pthread_mutex_destroy(&_mutex);
            (void)n;
        }

    private:
        pthread_mutex_t _mutex;
    };

    // 采用RAII风格进行锁的管理
    class LockGuard
    {
    public:
        LockGuard(Mutex &mutex):_mutex(mutex)
        {
            _mutex.Lock();
        }

        ~LockGuard()
        {
            _mutex.Unlock();
        }

    private:
        Mutex &_mutex;
    };
}

#endif