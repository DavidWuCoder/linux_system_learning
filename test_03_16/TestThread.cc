#include <iostream>
#include <string>
#include <pthread.h>
#include <unistd.h>
// #include <thread>  // C++11 线程库
#include <vector>  // 用于存储线程对象

int flag = 100;

std::string FormatId(const pthread_t &tid)
{
    char id[64];
    snprintf(id, sizeof(id), "0x%lx", tid);
    std::string s = id;
    return s;
}

void* routine(void* args)
{
    pthread_t tid = pthread_self();
    std::string name = static_cast<const char*>(args);
    int cnt = 5;
    while (cnt--)
    {
        flag++;
        std::cout << "我是一个新线程, name: " << name << "我的id是: " << FormatId(tid) << std::endl;
        sleep(1);
    }
    return (void*)100;
}

void Showid(pthread_t& id)
{
    printf("tid: 0x%lx\n", id);
}

int main()
{
    pthread_t id;
    int n = pthread_create(&id, nullptr, routine, (void *)"thread -1");
    (void)n;
    int cnt = 5;
    while(cnt--) {
        std::cout << "我是一个主线程" <<", cnt:"<< flag <<", name: " << "我的id是: " << FormatId(pthread_self()) << std::endl;
        sleep(1);
    }

    Showid(id);

    void* ret = nullptr;
    pthread_join(id, &ret);
    std::cout << "ret is" << (long long int)ret << std::endl;
}








// // 线程函数
// void hello(int id) {
//     while (true) {
//         std::cout << "Hello from thread " << id << "!" << std::endl;
//         sleep(1);
//     }
// }

// int main() {
//     std::thread t(hello, 1);  // 创建线程并传递参数

//     while(true)
//     {
//         std::cout << "我是主线程" << std::endl;
//         sleep(1);
//     }
//     // t.join();  // 等待子线程结束
//     return 0;
// }

// void *threadrun(void* args)
// {
//     std::string name = (char*)args;
//     while (true)
//     {
//         std::cout << "我是新线程，name：" << name << std::endl;
//         sleep(1);
//     }
//     return nullptr;
// }

// int main()
// {
//     pthread_t tid;
//     pthread_create(&tid, nullptr, threadrun, (void*)"thread-1");
//     while (true)
//     {
//         std::cout << "我是主线程" << std::endl;
//         sleep(1);
//     }
//     return 0;
// }