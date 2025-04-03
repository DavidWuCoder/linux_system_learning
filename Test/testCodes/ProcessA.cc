#include <iostream>
#include <sys/shm.h>
#include <cstring>

const char * pathname = "file";
const int projid = 0x66;
const int Size = 4096;

int main()
{
    key_t k = ftok(pathname, projid);
    int shmid = shmget(k, Size, IPC_CREAT | IPC_EXCL | 0666);

    char* str = (char *)shmat(shmid, nullptr, 0);

    strcpy(str, "I am process A !");
    printf("数据已经写入\n");

    shmdt(str);
    return 0;
}