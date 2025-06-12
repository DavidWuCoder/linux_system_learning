#include <sys/select.h>

#include <iostream>

int main()
{
    fd_set fset;
    std::cout << sizeof(fset) * 8 << std::endl;
    return 0;
}
