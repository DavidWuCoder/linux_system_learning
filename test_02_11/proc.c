#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
  pid_t id = fork();

  if (id == 0) {
    // child
    int cnt = 5;
    while (cnt) {
      printf("我是一个子进程，pid: %d, ppid: %d \n", getpid(), getppid());
      sleep(1);
      cnt--;
    }
    exit(0);
  }

  // parent
  // sleep(100);
  pid_t rid = wait(NULL);
  if (rid > 0) {
    printf("wait success, rid: %d\n", rid);
  }

  return 0;
}

// int main() {
//   // printf("hello world\n");
//
//   // FILE *fp = fopen("log.txt", "r");
//   // if (fp == NULL) {
//   //   return errno;
//   // }
//   // // 读取
//   // fclose(fp);
//
//   // int i = 0;
//   // for (int i = 0; i < 200; i++) {
//   //   printf("%d -> %s \n", i, strerror(i));
//   // }
//
//   // return errno;
//   //
//
//   int a = 10;
//   a /= 0;
//   return 89;
// }
