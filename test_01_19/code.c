#include <stdio.h>
#include <unistd.h>

extern char **environ;

int main(int argc, char *argv[]) {

  // 只是为了不让编辑器报警
  (void)argc;
  (void)argv;

  pid_t id = fork();

  if (id == 0) {
    // 子进程
    for (int i = 0; environ[i]; i++) {
      printf("environ[%d]: %s\n", i, environ[i]);
    }
  }

  sleep(3);
  return 0;
}
