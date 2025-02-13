#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <unistd.h>

#define COMMAND_SIZE 1024
#define HOST_NAME_MAX 1024
#define FORMAT "[%s@%s %s]# "

const char *GetUserName() {
  const char *name = getenv("USER");
  return name == NULL ? "None" : name;
}

const char *GetHostName() {
  static char hostname[HOST_NAME_MAX +
                       1]; // 静态数组，避免栈溢出并允许函数返回后依然有效
  if (gethostname(hostname, sizeof(hostname)) == 0) {
    return hostname;
  } else {
    return "None"; // 或者根据需要返回其他错误信息
  }
}

const char *GetPwd() {
  const char *pwd = getenv("PWD");
  return pwd == NULL ? "None" : pwd;
}

void MakeCommandLine(char cmd_prompt[], int size) {
  snprintf(cmd_prompt, size, FORMAT, GetUserName(), GetHostName(), GetPwd());
}

void PrintCommandPrompt() {
  char prompt[COMMAND_SIZE];
  MakeCommandLine(prompt, sizeof(prompt));
  printf("%s", prompt);
  fflush(stdout);
}

bool GetCommandLine(char *out, int size) {
  char *c = fgets(out, size, stdin);

  if (c == NULL) {
    return 1;
  }
  if (strlen(out) == 0)
    return false;
  out[strlen(out) - 1] = 0;
  return true;
}

int main() {
  while (true) {
    // 1.输出命令行提示符
    PrintCommandPrompt();

    // 2.获取用户输入的命令
    char commandline[COMMAND_SIZE];
    if (!GetCommandLine(commandline, sizeof(commandline))) {
      continue;
    }
    printf("echo %s\n", commandline);
  }
  return 0;
}
