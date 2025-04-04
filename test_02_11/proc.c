#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

typedef void (*func_t)();

#define NUM 5
func_t handlers[NUM];

void Download() { printf("我是一个下载任务！\n"); }
void Flush() { printf("我是一个刷新任务！\n"); }
void Log() { printf("我是一个记录日志任务！\n"); }

void registerHandlers(func_t h[], func_t f) {
  int i = 0;
  for (; i < NUM; i++) {
    if (h[i] == NULL) {
      break;
    }
  }
  if (i == NUM) {
    return;
  }
  h[i] = f;
  h[i + 1] = NULL;
}

int main() {
  pid_t id = fork();

  registerHandlers(handlers, Download);
  registerHandlers(handlers, Flush);
  registerHandlers(handlers, Log);

  if (id == 0) {
    // child
    int cnt = 5;
    while (1) {
      printf("我是一个子进程，pid: %d, ppid: %d \n", getpid(), getppid());
      sleep(1);
      cnt--;
    }
    exit(10);
  }

  while (1) {
    int status = 0;
    pid_t rid = waitpid(id, &status, WNOHANG);
    if (rid > 0) {
      printf("wait success, rid: %d, exit code: %d, exit_signal: %d\n", rid,
             WEXITSTATUS(status), status & 0x7f);
      break;
    } else if (rid == 0) {
      // 函数指针回调处理
      int i = 0;
      for (; handlers[i]; i++) {
        handlers[i]();
      }
      printf("本轮调用结束，子进程没有退出\n");
      sleep(1);
    } else {
      printf("等待失败\n");
      break;
    }
  }

  // int status = 0;
  //
  // // parent
  // // sleep(100);
  // pid_t rid = waitpid(id, &status, 0);
  // if (rid > 0) {
  //   if (WIFEXITED(status))
  //     printf("wait success, rid: %d, exit code: %d, exit signal: %d\n", rid,
  //            WEXITSTATUS(status), status & 0x7f);
  //   else
  //     printf("子进程退出异常\n");
  // }

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
