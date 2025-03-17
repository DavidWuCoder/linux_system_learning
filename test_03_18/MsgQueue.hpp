#ifndef MSGQUEUE_HPP
#define MSGQUEUE_HPP

#include <iostream>
#include <string>
#include <cstring>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define PATHNAME "./"
#define PROJID 12345

const int default_fd = -1;
const int default_size = 1024;

#define GET_MSGQUEUE (IPC_CREAT)
#define CREATE_MSGQUEUE (IPC_CREAT | IPC_EXCL)

class MsgQueue
{
    // 有类型数据块
    struct msgbuf {
        long mtype;
        char mtext[default_size];
    };
public:
    MsgQueue(int size):_msgfd(default_fd) {}

    // Create MsgQueue
    void Create(int flag)
    {
        // 唯一键值
        key_t key = ftok(PATHNAME, PROJID);
        if (key == -1)
        {
            std::cerr << "ftok error" << std::endl;
            exit(1);
        }
        
        // 创建 --- 暂时
        _msgfd = msgget(key, flag| 0666);
        if(_msgfd == -1)
        {
            std::cerr << "msgget error" << std::endl;
            exit(2);
        }
        std::cout << "MsgQueue created! fd: "<< _msgfd<< std::endl;
    }

    // 发送消息
    void Send(int type, const std::string text)
    {
        struct msgbuf msg;
        memset(&msg, 0, sizeof(msg));
        msg.mtype = type;
        memcpy(msg.mtext, text.c_str(), text.size());

        int n = msgsnd(_msgfd, &msg, sizeof(msg.mtext), 0);
        if (n == -1)
        {
            std::cerr << "send error" << std::endl;
            return;
        }
    }

    void Recv(int type, std::string& text)
    {
        struct msgbuf msg;
        memset(&msg, 0, sizeof(msg));

        int n = msgrcv(_msgfd, &msg, sizeof(msg.mtext), type, 0);
        if (n == -1)
        {
            std::cerr << "msgrcv error" << std::endl;
            return;
        }
        text = msg.mtext;
    }

    // destroy the queue
    void Destroy()
    {
        int n = msgctl(_msgfd, IPC_RMID, 0);
        if (n == -1)
        {
            std::cerr << "msgctl error" << std::endl;
            return ;
        }
        std::cout << "MsgQueue destroyed" << std::endl;
    }

    ~MsgQueue() 
    {}
private:
    int _msgfd;

};

// 定义消息类型
#define MSG_TYPE_CLIENT 1
#define MSG_TYPE_SERVER 1

class Server: public MsgQueue
{
public:
    Server() :MsgQueue(1024)
    {
        MsgQueue::Create(CREATE_MSGQUEUE);
        std::cout << "server create msgqueue done" << std::endl;
    }

    ~Server()
    {
        MsgQueue::Destroy();
    }

};

class Client: public MsgQueue
{
public:
    Client() :MsgQueue(1024)
    {
        MsgQueue::Create(GET_MSGQUEUE);
        std::cout << "client get msgqueue success" << std::endl;
    }

    ~Client()
    {}

};


#endif