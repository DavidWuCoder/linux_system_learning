#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
    // if (argc != 2)
    //     exit(1);
    // int fd = open(argv[1], O_RDONLY);
    // if (fd < 0)
    //     exit(1);
    //
    // dup2(fd, 0);
    // close(fd);
    //
    // while (1)
    // {
    //     char buffer[64];
    //     if (!fgets(buffer, sizeof(buffer), stdin))
    //         break;
    //     printf("%s", buffer);
    // }
    //
    close(1);
    int fd = open("log.txt", O_CREAT | O_WRONLY | O_APPEND, 0666);
    printf("%d", fd);
    printf("hello world!\n");
    printf("hello world!\n");
    printf("hello world!\n");
    const char* msg = "hello world!\n";
    write(fd, msg, strlen(msg));

    fflush(stdout);

    close(fd);

    // printf("fd1:%d\n", fd);
    // printf("hello world!\n");
    // printf("hello world!\n");
    // printf("hello world!\n");
    // fprintf(stdout, "hello !\n");
    // fprintf(stdout, "hello !\n");
    // fprintf(stdout, "hello !\n");
    /*printf("stdin: %d\n", stdin->_fileno);*/
    /*printf("stdout: %d\n", stdout->_fileno);*/
    /*printf("stderr: %d\n", stderr->_fileno);*/
    /**/
    /*umask(0);*/
    /*int fd1 = open("log1.txt", O_CREAT | O_WRONLY | O_TRUNC, 0666);*/
    /*int fd2 = open("log2.txt", O_CREAT | O_WRONLY | O_TRUNC, 0666);*/
    /*int fd3 = open("log3.txt", O_CREAT | O_WRONLY | O_TRUNC, 0666);*/
    /*int fd4 = open("log4.txt", O_CREAT | O_WRONLY | O_TRUNC, 0666);*/
    /**/
    /*if (fd1 < 0)*/
    /*    exit(-1);*/
    /*if (fd2 < 0)*/
    /*    exit(-1);*/
    /*if (fd3 < 0)*/
    /*    exit(-1);*/
    /*if (fd4 < 0)*/
    /*    exit(-1);*/
    /**/
    /*printf("%d\n", fd1);*/
    /*printf("%d\n", fd2);*/
    /*printf("%d\n", fd3);*/
    /*printf("%d\n", fd4);*/
    /**/
    /*close(fd1);*/
    /*close(fd2);*/
    /*close(fd3);*/
    /*close(fd4);*/
    // int fd = open("log.txt", O_RDONLY);
    // while (1)
    // {
    //     char buffer[64];
    //     memset(buffer, 0, sizeof(buffer));
    //     int n = read(fd, buffer, sizeof(buffer));
    //     printf("%s", buffer);
    //     if (n == 0)
    //     {
    //         break;
    //     }
    // }
    // int fd = open("log.txt", O_CREAT | O_WRONLY | O_TRUNC, 0666);
    // if (fd < 0)
    // {
    //     perror("open!\n");
    //     return 1;
    // }
    // printf("%d\n", fd);
    // const char *msg = "abd\n";
    //
    // int cnt = 1;
    // while (cnt--)
    // {
    //     write(fd, msg, strlen(msg));
    // }

    return 0;
}
