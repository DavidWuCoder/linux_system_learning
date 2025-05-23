#ifndef _THREAD_H_
#define _THREAD_H_

#include <iostream>
#include <string>
#include <pthread.h>
#include <cstdio>
#include <cstring>
#include <cstdint>
#include <functional>
#include "Log.hpp"
using namespace LogModule;


namespace ThreadModule
{
    static uint32_t number = 0;

    class Thread
    {
        using func_t = std::function<void()>;
    public:
        Thread(func_t func) : _func(func), _tid(0), _isdetach(false), _isrunning(false), res(nullptr)
        {
            _name = "thread-" + std::to_string(number++);
        }

        void Detach()
        {
            if (_isdetach)
            {
                return ;
            }
            if (_isrunning)
                pthread_detach(_tid);
            EnableDetach();
            std::cout << "线程已经被分离" << std::endl;
        }

        void EnableDetach()
        {
            if (_isrunning) return;
            _isdetach = true;
        }

        void EnableRunning()
        {
            _isrunning = true;
        }

        static void* Routine(void* args)
        {
            Thread* self = static_cast<Thread *>(args);
            if (self->_isdetach)
                self->Detach();
            pthread_setname_np(self->_tid, self->_name.c_str());
            self->_func();
            return nullptr;
        }

        void Start()
        {
            if (_isrunning)
                return ;
            int n = pthread_create(&_tid, nullptr, Routine, this);
            // pthread_setname_np(_tid, _name.c_str());
            if (n != 0)
            {
                std::cerr << "create thread error: " << strerror(n) << std::endl;
            }
            EnableRunning();
            if (_isdetach)
            {
                Detach();
            }

        }

        bool Stop()
        {
            if (_isrunning)
            {
                int n = pthread_cancel(_tid);
                if (n != 0)
                {
                    std::cerr << "create thread error: " << strerror(n) << std::endl;
                    return false;
                }
                else
                {
                    _isrunning = false;
                    std::cout << _name << "_stop" << std::endl;
                }
            }
            return true;
        }

        void Join()
        {
            if (_isdetach)
            {
                std::cout << "线程已经分离，不能join" <<std::endl;
                return;
            }
            int n = pthread_join(_tid, &res);
            if (n != 0)
            {
                LOG(LogLevel::INFO) << "join failed";
            }
            else
            {
                LOG(LogLevel::INFO) << "join success";
            }
        }

        std::string Name()
        {
            return _name;
        }

        pthread_t Id()
        {
            return _tid;
        }

        ~Thread() {}

    private:
        pthread_t _tid;
        std::string _name;
        bool _isdetach;
        bool _isrunning;
        void* res;
        func_t _func;
    };
}

#endif