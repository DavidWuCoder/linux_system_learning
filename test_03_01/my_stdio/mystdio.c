#include "mystdio.h"

#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static MYFILE *BuyFILE(int fd, int flag)
{
    MYFILE *f = (MYFILE *)malloc(sizeof(MYFILE));
    if (f == NULL)
    {
        return NULL;
    }
    f->bufferlen = 0;
    f->fileno = fd;
    f->flag = flag;
    f->fush_method = Line_FLUSH;
    memset(f->outbuffer, 0, sizeof(f->outbuffer));
    return f;
}

MYFILE *MyFopen(const char *path, const char *mode)
{
    int fd = -1;
    int flag = 0;
    if (strcmp(mode, "w") == 0)
    {
        flag = O_CREAT | O_WRONLY | O_TRUNC;
        fd = open(path, flag, 0666);
    }
    else if (strcmp(mode, "a") == 0)
    {
        flag = O_CREAT | O_WRONLY | O_TRUNC;
        fd = open(path, flag, 0666);
    }
    else if (strcmp(mode, "r") == 0)
    {
        flag = O_RDONLY;
        fd = open(path, flag);
    }
    else
    {
        // to do
    }
    if (fd < 0)
        return NULL;
    MYFILE *f = BuyFILE(fd, flag);
    return f;
}
void MyFclose(MYFILE *file)
{
    if (file->fileno <= 0)
        return;
    MyFFlush(file);
    close(file->fileno);
    free(file);
}
int MyFwrite(MYFILE *file, void *str, int len)
{
    // 1.拷贝
    memcpy(file->outbuffer + file->bufferlen, str, len);
    file->bufferlen += len;

    // 2.尝试判断
    if ((file->fush_method & Line_FLUSH) &&
        file->outbuffer[file->bufferlen - 1] == '\n')
    {
        MyFFlush(file);
    }
    return 0;
}
void MyFFlush(MYFILE *file)
{
    if (file->bufferlen <= 0)
        return;
    int n = write(file->fileno, file->outbuffer, file->bufferlen);
    (void)n;
    fsync(file->fileno);
    file->bufferlen = 0;
}
