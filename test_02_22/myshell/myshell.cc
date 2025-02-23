#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <random>
#include <string>

#define COMMAND_SIZE 1024
#define HOST_NAME_MAX 1024
#define FORMAT "[%s@%s %s]# "

// 下面是shell定义的全局数据
#define MAXARGV 128
char *g_argv[MAXARGV];
int g_argc;

// 环境变量表
#define MAX_ENVS 100
char *g_env[MAX_ENVS];
int g_envs = 0;

// for test
char cwd[1024];
char cwdenv[9999];

// last status code
int lastcode = 0;

// 重定向内容
#define NONE_REDIR 0
#define INPUT_REDIR 1
#define OUTPUT_REDIR 2
#define APPED_REDIR 3

int redir = NONE_REDIR;
std::string filename;

const char *GetUserName()
{
    const char *name = getenv("USER");
    return name == NULL ? "None" : name;
}

const char *GetHostName()
{
    static char hostname[HOST_NAME_MAX +
                         1];  // 静态数组，避免栈溢出并允许函数返回后依然有效
    if (gethostname(hostname, sizeof(hostname)) == 0)
    {
        return hostname;
    }
    else
    {
        return "None";  // 或者根据需要返回其他错误信息
    }
}

const char *GetPwd()
{
    // const char *pwd = getenv("PWD");
    const char *pwd = getcwd(cwd, sizeof(cwd));
    if (pwd != NULL)
    {
        snprintf(cwdenv, sizeof(cwdenv), "PWD=%s", cwd);
        putenv(cwdenv);
        return pwd;
    }
    return "None";
}

const char *GetHome()
{
    const char *home = getenv("HOME");
    return home == NULL ? "" : home;
}

void InitEnv()
{
    extern char **environ;
    memset(g_env, 0, sizeof(g_env));
    g_envs = 0;

    // 本来要从配置文件来读取
    for (int i = 0; environ[i]; i++)
    {
        // 1.1申请空间
        g_env[i] = (char *)malloc(strlen(environ[i]) + 1);
        strcpy(g_env[i], environ[i]);
        g_envs++;
    }
    g_env[g_envs++] = (char *)"HAA=for_tset";
    g_env[g_envs] = NULL;
    for (int i = 0; g_env[i]; i++)
    {
        putenv(g_env[i]);
    }
}

// command : cd
bool Cd()
{
    if (g_argc == 1)
    {
        std::string home = GetHome();
        if (home.empty())
        {
            return true;
        }
        chdir(home.c_str());
    }
    else
    {
        std::string where = g_argv[1];
        chdir(where.c_str());
    }
    return true;
}

bool Echo()
{
    std::string opt = g_argv[1];
    if (opt == "$?")
    {
        std::cout << lastcode << std::endl;
        lastcode = 0;
    }
    else if (opt[0] == '$')
    {
        std::string env_name = opt.substr(1);
        const char *env_value = getenv(env_name.c_str());
        if (env_value)
        {
            std::cout << env_value << std::endl;
        }
    }
    else
    {
        std::cout << opt << std::endl;
    }
    return true;
}

std::string DirName(const char *pwd)
{
#define SLASH "/"
    std::string dir = pwd;
    if (dir == SLASH)
        return SLASH;
    auto pos = dir.rfind(SLASH);
    if (pos == std::string::npos)
        return "Bug";
    return dir.substr(pos + 1);
}

void MakeCommandLine(char cmd_prompt[], int size)
{
    snprintf(cmd_prompt, size, FORMAT, GetUserName(), GetHostName(),
             DirName(GetPwd()).c_str());
}

void PrintCommandPrompt()
{
    char prompt[COMMAND_SIZE];
    MakeCommandLine(prompt, sizeof(prompt));
    printf("%s", prompt);
    fflush(stdout);
}

bool GetCommandLine(char *out, int size)
{
    char *c = fgets(out, size, stdin);

    if (c == NULL)
    {
        return 1;
    }
    if (strlen(out) == 0)
        return false;
    out[strlen(out) - 1] = 0;
    return true;
}

// 3.命令行分析
bool CommandParse(char *commandline)
{
#define SEP " "
    g_argc = 0;
    g_argv[g_argc++] = strtok(commandline, SEP);

    while ((bool)(g_argv[g_argc++] = strtok(nullptr, SEP)));
    g_argc--;
    return g_argc > 0 ? true : false;
}

void PrintArgv()
{
    for (int i = 0; g_argv[i]; i++)
    {
        printf("argv[%d]->%s\n", i, g_argv[i]);
    }
    printf("argc: %d\n", g_argc);
}

int CheckAndExecBuiltin()
{
    std::string cmd = g_argv[0];
    if (cmd == "cd")
    {
        Cd();
        return true;
    }
    else if (cmd == "echo")
    {
        Echo();
        return true;
    }
    return false;
}

int Excute()
{
    pid_t id = fork();
    if (id == 0)
    {
        int fd = -1;
        // child
        if (redir == INPUT_REDIR)
        {
            fd = open(filename.c_str(), O_RDONLY);
            if (fd < 0)
                exit(1);
            dup2(fd, 0);
            close(fd);
        }
        else if (redir == OUTPUT_REDIR)
        {
            fd = open(filename.c_str(), O_CREAT | O_WRONLY | O_TRUNC, 0666);
            if (fd < 0)
                exit(2);
            dup2(fd, 1);
            close(fd);
        }
        else if (redir == APPED_REDIR)
        {
            fd = open(filename.c_str(), O_CREAT | O_WRONLY | O_APPEND, 0666);
            if (fd < 0)
                exit(3);
            dup2(fd, 1);
            close(fd);
        }
        else
        {
        }
        execvp(g_argv[0], g_argv);
        exit(1);
    }
    // father
    int status;
    pid_t rid = waitpid(id, &status, 0);
    if (rid > 0)
    {
        lastcode = WEXITSTATUS(status);
    }
    return 0;
}

void TrimSpace(char cmd[], int &end)
{
    while (std::isspace(cmd[end]))
    {
        end++;
    }
}

void RedirCheck(char *cmd)
{
    redir = NONE_REDIR;
    filename.clear();

    int start = 0;
    int end = strlen(cmd);

    while (end-- > start)
    {
        if (cmd[end] == '<')
        {
            cmd[end++] = 0;
            TrimSpace(cmd, end);
            redir = INPUT_REDIR;
            filename = cmd + end;
            break;
        }
        else if (cmd[end] == '>')
        {
            if (cmd[end - 1] == '>')
            {
                cmd[end - 1] = 0;
                redir = APPED_REDIR;
            }
            else
            {
                redir = OUTPUT_REDIR;
            }
            cmd[end++] = 0;
            TrimSpace(cmd, end);
            filename = cmd + end;
            break;
        }
    }
}

int main()
{
    // shell 启动的时候，从系统中获取环境变量
    // 为方便，这里直接从父进程获取
    InitEnv();

    while (true)
    {
        // 1.输出命令行提示符
        PrintCommandPrompt();

        // 2.获取用户输入的命令
        char commandline[COMMAND_SIZE];
        if (!GetCommandLine(commandline, sizeof(commandline)))
        {
            continue;
        }
        // printf("echo %s\n", commandline);

        // 3.命令行分析
        RedirCheck(commandline);

        // printf("redir: %d, filename: %s\n", redir, filename.c_str());

        // 4.命令行分析
        if (!CommandParse(commandline))
        {
            continue;
        }
        // PrintArgv();

        // 5.检测是否为内建命令
        if (CheckAndExecBuiltin())
        {
            continue;
        }

        // 6.执行命令
        Excute();
    }
    // cleanup
    return 0;
}
