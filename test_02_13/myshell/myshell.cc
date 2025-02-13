#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sys/wait.h>
#include <unistd.h>

#define COMMAND_SIZE 1024
#define HOST_NAME_MAX 1024
#define FORMAT "[%s@%s %s]# "

// 下面是shell定义的全局数据
#define MAXARGV 128
char *g_argv[MAXARGV];
int g_argc;

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

std::string DirName(const char *pwd) {
#define SLASH "/"
  std::string dir = pwd;
  if (dir == SLASH)
    return SLASH;
  auto pos = dir.rfind(SLASH);
  if (pos == std::string::npos)
    return "Bug";
  return dir.substr(pos + 1);
}

void MakeCommandLine(char cmd_prompt[], int size) {
  snprintf(cmd_prompt, size, FORMAT, GetUserName(), GetHostName(),
           DirName(GetPwd()).c_str());
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

// 3.命令行分析
bool CommandParse(char *commandline) {
#define SEP " "
  g_argc = 0;
  g_argv[g_argc++] = strtok(commandline, SEP);

  while ((bool)(g_argv[g_argc++] = strtok(nullptr, SEP)))
    ;
  g_argc--;
  return true;
}

void PrintArgv() {
  for (int i = 0; g_argv[i]; i++) {
    printf("argv[%d]->%s\n", i, g_argv[i]);
  }
  printf("argc: %d\n", g_argc);
}

int Excute() {
  pid_t id = fork();
  if (id == 0) {
    // child
    execvp(g_argv[0], g_argv);
    exit(1);
  }
  // father
  pid_t rid = waitpid(id, nullptr, 0);
  (void)rid; // 让rid使用一下
  return 0;
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
    // printf("echo %s\n", commandline);

    // 3.命令行分析
    CommandParse(commandline);
    // PrintArgv();

    // 4.执行命令
    Excute();
  }
  return 0;
}
