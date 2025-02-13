#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
  printf("进程开始运行！\n");
  if (fork() == 0) {
    printf("My pid is %d\n", getpid());
    sleep(1);

    char *const argv[] = {(char *const)"ls", (char *const)"-a",
                          (char *const)"-l", NULL};
    execv("/usr/bin/ls", argv);

    // execlp("ls", "-a", "-l", NULL);

    // execl("./other", "other", NULL);
  }
  waitpid(-1, NULL, 0);
  printf("进程结束！\n");
  return 0;
}
