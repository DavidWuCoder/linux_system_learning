#include <cstdio>
#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

#define ERR_EXIT(m)         \
    do                      \
    {                       \
        perror(m);          \
        exit(EXIT_FAILURE); \
    } while (0)

const int gdefaultid = -1;
const int gsize = 4096;
const std::string pathname = ".";
const int projid = 0x66;
const int gmod = 0666;
#define CREATER "creater"
#define USER "user"

class Shm
{
private:
    bool CreateHelper(int flg)
    {
        key_t k = ftok(pathname.c_str(), projid);
        if (k < 0)
        {
            ERR_EXIT("ftok");
        }
        printf("key: 0x%x\n", k);
        // 共享内存的生命周期随内核
        // 
        _shmid = shmget(k, _size, flg);
        if (_shmid < 0)
        {
            ERR_EXIT("shmget");
        }
        printf("shmid: %d\n", _shmid);
        return true;
    }
public:
    Shm()
        : _shmid(gdefaultid), _size(gsize), _start_mem(nullptr)
    {
    }

    void Create()
    {
        CreateHelper(IPC_CREAT | IPC_EXCL | 0666);
    }

    void Get()
    {
        CreateHelper(IPC_CREAT);
    }

    void Destroy()
    {
        if (_shmid == -1)
        {
            return;
        }
        int n = shmctl(_shmid, IPC_RMID, nullptr);
        if (n == 0)
        {
            printf("shmctl delete shm: %d success!\n", _shmid);
        }
        else
        {
            ERR_EXIT("shmctl");
        }
    }

    void Attach()
    {
        _start_mem = shmat(_shmid, nullptr, 0);
        if ((long long)_start_mem < 0)
        {
            ERR_EXIT("shmat");
        }
        printf("attach success!\n");
    }

    void *VituralAddr()
    {
        printf("VirtualAddr:%p\n", _start_mem);
        return _start_mem;
    }

    ~Shm()
    {
    }

private:
    int _shmid;
    key_t k;
    int _size;
    void * _start_mem;
    std::string usertype;
};