#include <stdio.h>
#include <unistd.h>

int gval = 100;

int main() {
  printf("父进程开始运行， pid:%d\n", getpid());
  pid_t id = fork();
  if (id < 0) {
    perror("fork");
    return 1;
  } else if (id == 0) {
    // child process
    // printf("我是一个子进程，我的pid：%d, 我的父进程pid：%d, gval = %d\n",
    //        getpid(), getppid(), gval);
    // sleep(5);
    while (1) {
      sleep(1);
      printf("我是一个子进程，我的pid：%d, 我的父进程pid：%d, gval = %d\n",
             getpid(), getppid(), gval);
      printf("子进程修改变量， %d -> %d", gval, gval + 10);
      gval += 10;
    }
  } else {
    // father  process
    while (1) {
      sleep(1);
      printf("我是一个父亲进程，我的pid：%d, 我的父进程pid：%d, gva = %d\n",
             getpid(), getppid(), gval);
    }
  }

  return 0;
}

// printf("父进程开始运行， pid:%d\n", getpid());
// fork();
// printf("进程开始运行， pid:%d\n", getpid());
// while (1) {
//   printf("我是一个进程！PID: %d, PPID: %d\n", getpid(), getppid());
//   sleep(1);
// }
