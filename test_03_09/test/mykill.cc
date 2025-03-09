#include <iostream>
#include <string>
#include <sys/types.h>
#include <signal.h>

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        std::cout << "./mykill signumber id" << std::endl;
        return 1;
    }
    int signum = std::stoi(argv[1]);
    pid_t target = std::stoi(argv[2]);

    int n = kill(target, signum);

    if (n == 0)
    {
        std::cout << "send " << signum << " to process[" << target << "]successs !" << std::endl;
    }
    return 0;   
}