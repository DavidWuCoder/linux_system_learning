#pragma once

#include <stdio.h>

#define MAX 1024
#define NONE_FLUSH (1 << 0)
#define Line_FLUSH (1 << 1)
#define FULL_FLUSH (1 << 2)

typedef struct IO_FILE
{
    int flag;
    char outbuffer[MAX];
    int bufferlen;
    int fileno;
    int fush_method;
} MYFILE;

MYFILE *MyFopen(const char *path, const char *mode);
void MyFclose(MYFILE *);
int MyFwrite(MYFILE *, void *str, int len);
void MyFFlush(MYFILE *);
