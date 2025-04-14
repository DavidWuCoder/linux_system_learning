#pragma once
#include <iostream>
#include <string>
#include <queue>
#include "Log.hpp"
#include "Thread.hpp"
#include "Cond.hpp"
#include "Mutex.hpp"
#include <vector>

namespace ThreadPoolModule
{
    using namespace ThreadModule;
    using namespace LogModule;
    using namespace CondModule;
    using namespace MutexModule;
    static const int gnum = 5;
    template <class T>
    class ThreadPool
    {
    private:
        void WakeAllThread()
        {
            LockGuard lockguard(_mutex);
            if (_sleepernum > 0)
                _cond.BroadCast();
            LOG(LogLevel::INFO) << "唤醒所有线程";
        }

        void WakeUpOne()
        {
            _cond.Signal();
        }

        ThreadPool(int num = gnum)
            : _num(num), _is_running(false), _sleepernum(0)
        {
            for (int i = 0; i < num; i++)
            {
                _threads.emplace_back(
                    [this]()
                    {
                        HandlerTask();
                    });
            }
        }

        void Start()
        {
            if (_is_running)
                return;
            _is_running = true;
            for (auto &thread : _threads)
            {
                thread.Start();
            }
        }

        ThreadPool(const ThreadPool<T> &) = delete;
        ThreadPool<T> &operator=(const ThreadPool<T> &) = delete;

    public:
        static ThreadPool<T> *GetInstance()
        {
            if (inc == nullptr)
            {
                LockGuard lockguard(_lock);
                LOG(LogLevel::DEBUG) << "获取单例";
                if (inc == nullptr)
                {
                    LOG(LogLevel::DEBUG) << "首次使用单例，创建 ";
                    inc = new ThreadPool<T>();
                    inc->Start();
                }
            }

            return inc;

        }

        void Stop()
        {
            if (!_is_running)
                return;
            _is_running = false;
            // 唤醒所有线程
            WakeAllThread();
        }

        void Join()
        {
            for (auto &thread : _threads)
            {
                thread.Join();
            }
        }

        void HandlerTask()
        {
            char name[64];
            pthread_getname_np(pthread_self(), name, sizeof(name));
            while (true)
            {
                T t;
                {
                    LockGuard lockguard(_mutex);
                    // 队列为空 且 线程池没有退出才休眠
                    while (_taskq.empty() && _is_running == true)
                    {
                        _sleepernum++;
                        // LOG(LogLevel::DEBUG) << _sleepernum;
                        _cond.Wait(_mutex);
                        _sleepernum--;
                    }
                    // 内部线程被唤醒
                    // LOG(LogLevel::DEBUG) << "一个线程被唤醒";
                    if (!_is_running && _taskq.empty())
                    {
                        LOG(LogLevel::INFO) << name << " 退出了，线程池退出且任务队列为空";
                        break;
                    }

                    // 一定有任务
                    t = _taskq.front();
                    _taskq.pop();
                }
                t(); // 处理任务不能在临界区执行
            }
        }

        bool Enqueue(const T &in)
        {
            if (_is_running)
            {
                LockGuard lockguard(_mutex);
                _taskq.push(in);
                // LOG(LogLevel::DEBUG) << "size : " << _threads.size();
                // 唤醒线程
                if (_sleepernum == _threads.size())
                {
                    LOG(LogLevel::DEBUG) << "唤醒一个线程";
                    WakeUpOne();
                }
                return true;
            }
            return false;
        }

        ~ThreadPool()
        {
        }

    private:
        int _num; // 线程个数
        std::vector<Thread> _threads;
        std::queue<T> _taskq;

        Cond _cond;
        Mutex _mutex;

        bool _is_running;
        int _sleepernum;

        //
        static ThreadPool<T> *inc;
        static Mutex _lock;
    };

    template<typename T>
    ThreadPool<T> *ThreadPool<T>::inc = nullptr;

    template<typename T>
    Mutex ThreadPool<T>::_lock;
}