#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[], char *env[]) {

  // 只是为了不让编辑器报警
  (void)argc;
  (void)argv;
  (void)env;

  const char *UserName = getenv("USER");
  if (UserName == NULL)
    return 1;

  if (strcmp(UserName, "infinite") == 0) {
    printf("hello %s!\n", UserName);
  }

  return 0;
}
