#include <iostream>
#include <string>
#include <pthread.h>
#include <unistd.h>
#include <cstring>
#include <vector>

//创建多线程
const int num = 10;

void *routine(void* args)
{
    sleep(1);
    std::string name = static_cast<const char*>(args);
    int cnt = 5;
    while(cnt--)
    {
        std::cout << "新线程名字： " << name << std::endl;
        sleep(1);
    }
    return (void*)10;
}

int main()
{
    std::vector<pthread_t> tids;
    for (int i = 0; i < num; i++)
    {
        pthread_t tid;
        char *id = new char[64];
        snprintf(id, 64, "thread-%d", i);
        int n = pthread_create(&tid, nullptr, routine, id);
        if (n == 0)
            tids.push_back(tid);
    }

    for (int i = 0; i < num; i++)
    {
        // 一个一个等待
        int n = pthread_join(tids[i], nullptr);
        if (n == 0);
        {
            std::cout << "等待新线程成功" << std::endl;
        }
    }

    return 0;
}

// class Task
// {
// public:
//     Task(int a, int b) : _a(a), _b(b) {}
//     int Excute()
//     {
//         return _a + _b;
//     }
//     ~Task() {}
// private:
//     int _a;
//     int _b;
// };

// class Result
// {
// public:
//     Result(int result):_result(result) {}
//     int GetResult()
//     {
//         return _result;
//     }
//     ~Result() {}

// private:
//     int _result;
// };

// // 线程不能用exit(),因为整个进程都会终止
// void *routine(void *args)
// {
//     pthread_detach(pthread_self());
//     std::cout << "新线程被分离" <<std::endl;
//     int cnt = 5;
//     while(cnt--)
//     {
//         std::cout << cnt << std::endl;
//         sleep(1);
//     }

//     // Task *t = static_cast<Task*>(args);
//     // sleep(1);
//     // Result *res = new Result(t->Excute());
//     // sleep(1);
//     // // return res;
//     // pthread_exit(res);// 等价
//     // std::cout << "新线程不应该看到这里" << std::endl;
//     return (void*)10;
// }

// int main()
// {
//     pthread_t tid;
//     Task *t = new Task(10, 20);
//     pthread_create(&tid, nullptr, routine, (void*)t);

//     // pthread_detach(tid);
//     // std::cout << "新线程被分离" <<std::endl;
//     sleep(1);
//     // int cnt = 5;
//     // while (cnt--)
//     // {
//     //     std::cout << "main线程名字： " << std::endl;
//     //     sleep(1);
//     // }
//     // sleep(1);
//     // pthread_cancel(tid);

//     // void* ret = nullptr;
//     // pthread_join(tid, );
//     int n = pthread_join(tid, nullptr);
//     if (n != 0)
//     {
//         std::cout << "pthread error" << strerror(n)<< std::endl;
//     }
//     else
//     {
//         std::cout << "pthread join success" << std::endl;
//     }

//     // std::cout << "新线程结束，退出码 " << (long long)ret << std::endl;
//     // Result *ret = nullptr;
//     // pthread_join(tid, (void**)&ret);
//     // int n = ret->GetResult();
//     // std::cout << "新线程结束，运行结果：" << n << std::endl;

//     // delete t;
//     // delete ret;

//     return 0;
// }