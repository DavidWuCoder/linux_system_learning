#include <stdio.h>
#include <unistd.h>

int main() {
  pid_t id = fork();
  if (id == 0) {
    // child
    while (1) {
      printf("I am a child process, I am running. pid: %d\n", getpid());
      sleep(1);
    }
  } else {
    // father
    int count = 5;
    while (count) {
      printf("I am a parent process, I am running. %d\n", count);
      sleep(1);
      count--;
    }
  }
  return 0;
}
