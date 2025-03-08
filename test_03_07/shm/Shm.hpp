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
const int gmode = 0666;
#define CREATER "creater"
#define USER "user"

class Shm
{
private:
    bool CreateHelper(int flg)
    {

        printf("key: 0x%x\n", _key);
        // 共享内存的生命周期随内核
        //
        _shmid = shmget(_key, _size, flg);
        if (_shmid < 0)
        {
            ERR_EXIT("shmget");
        }
        printf("shmid: %d\n", _shmid);
        return true;
    }

    void Create()
    {
        CreateHelper(IPC_CREAT | IPC_EXCL | gmode);
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

    void Detach()
    {
        int n = shmdt(_start_mem);
        if (n == 0)
        {
            std::cout << "Detach success!" << std::endl;
        }
    }

    void Get()
    {
        CreateHelper(IPC_CREAT);
    }

    void Destroy()
    {
        Detach();
        if (_shmid == -1)
        {
            return;
        }
        if (_usertype == CREATER)
        {            
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
    }


public:
    Shm(const std::string &pathname, int projid, const std::string usertype)
        : _shmid(gdefaultid), _size(gsize), _start_mem(nullptr), _usertype(usertype)
    {
        _key = ftok(pathname.c_str(), projid);
        if (_key < 0)
        {
            ERR_EXIT("ftok");
        }
        if (usertype == CREATER) Create();
        else if (usertype == USER)
        {
            Get();
        }
        Attach();
    }

    int Size()
    {
        return _size;
    }

    void *VituralAddr()
    {
        printf("VirtualAddr:%p\n", _start_mem);
        return _start_mem;
    }

    ~Shm()
    {
        if (_usertype == CREATER)
            Destroy();
    }

private:
    int _shmid;
    key_t _key;
    int _size;
    void *_start_mem;
    std::string _usertype;
};