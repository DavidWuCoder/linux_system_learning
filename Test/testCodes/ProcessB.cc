#include <iostream>
#include <sys/shm.h>
#include <cstring>

const char * pathname = "file";
const int projid = 0x66;
const int Size = 4096;

int main()
{
    key_t k = ftok(pathname, projid);
    int shmid = shmget(k, Size, IPC_CREAT);

    char* str = (char *)shmat(shmid, nullptr, 0);

    printf("Process A say : %s\n", str);

    shmdt(str);
    shmctl(shmid, IPC_RMID, NULL);
    return 0;
}