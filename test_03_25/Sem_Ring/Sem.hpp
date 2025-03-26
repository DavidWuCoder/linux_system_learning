#include <iostream>
#include <semaphore.h>
#include <pthread.h>

const int default_value = 0;

namespace SemModule
{
    class Sem
    {
    public:
        Sem(unsigned int sem_value = default_value)
        {
            sem_init(&_sem, 0, sem_value);
        }

        void P()
        {
            int n = sem_wait(&_sem);  // 原子的
            (void)n;
        }

        void V()
        {
            int n = sem_post(&_sem); // 原子的
        }

        ~Sem() 
        {
            sem_destroy(&_sem);
        }
    private:
        sem_t _sem;
    };
};