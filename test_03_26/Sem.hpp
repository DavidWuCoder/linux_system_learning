#ifndef SEM_HPP
#define SEM_HPP

#include <iostream>
#include <memory>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

const std::string PATH = "/tmp";
const int PROJ_ID = 12345;
const int default_num = 1;

#define GET_SEM (IPC_CREAT)
#define BUILD_SEM (IPC_CREAT | IPC_EXCL | 0666)

// 一个十进制转十六进制的函数
std::string intToHEX(int num)
{
    char hex[64];
    snprintf(hex, sizeof(hex), "0x%x", num);
    return hex;
}

class Semaphore
{
    void pv(int num)
    {
        struct sembuf sem_buf;
        sem_buf.sem_num = 0; // 信号量编号从0开始
        sem_buf.sem_op = num; // -1代表p操作, 1代表V操作
        sem_buf.sem_flg = SEM_UNDO; // 不关心
        int n = semop(_semid, &sem_buf, 1);
        if (n < 0)
        {
            std::cerr << "semop PV failed" << std::endl;
            exit(0);
        }
    }
public:
    Semaphore(int semid) : _semid(semid)
    {
    }

    int Id() const
    {
        return _semid;
    }

    void P()
    {
        pv(-1);
    }

    void V()
    {
        pv(1);
    }

    ~Semaphore()
    {
        if (_semid >= 0)
        {
            int n = semctl(_semid, 0, IPC_RMID);
            if (n < 0)
            {
                std::cerr << "semctl IPC_RMID failed" << std::endl;

            }
            std::cout << "Semaphore " << _semid << "removed" << std::endl;
        }
    }

private:
    int _semid;
};

// 简单的建造者模式
class SemaphoreBuilder
{
public:
    SemaphoreBuilder()
    {
    }

    SemaphoreBuilder &SetVal(int val)
    {
        _val = val;
        return *this;
    }

    std::shared_ptr<Semaphore> Build(int flag, int num = default_num)
    {
        if (_val < 0)
        {
            std::cout << "you must init first" << std::endl;
            return nullptr;
        }
        // 1.构建键值
        std::cout << "Build a semaphore" << std::endl;
        key_t key = ftok(PATH.c_str(), PROJ_ID);
        if (key < 0)
        {
            std::cerr << "ftok failed" << std::endl;
            return nullptr;
        }
        std::cout << "Got key" << intToHEX(key) << std::endl;

        // 创建信号量集
        int semid = semget(key, num, flag);
        if (semid < 0)
        {
            std::cerr << "semget failed" << std::endl;
            return nullptr;
        }

        std::cout << "Got semaphore id:" << semid << std::endl;

        // 3.初始化
        if (flag == BUILD_SEM)
            Init(semid, 0, _val);

        return std::make_shared<Semaphore>(semid);
    }

    ~SemaphoreBuilder()
    {
    }

private:
    int _val = 1; // 暂定

    int Init(int semid, int num, int val)
    {
        // 需要手动定义这个结构体
        union semun
        {
            int val;               /* Value for SETVAL */
            struct semid_ds *buf;  /* Buffer for IPC_STAT, IPC_SET */
            unsigned short *array; /* Array for GETALL, SETALL */
            struct seminfo *__buf; /* Buffer for IPC_INFO
                                      (Linux-specific) */
        } un;
        un.val = val;

        int n = semctl(semid, num, SETVAL, un);
        if (n < 0)
        {
            std::cerr << "semctl SETVAL failed" << std::endl;
            return false;
        }
        return true;
    }
};

#endif