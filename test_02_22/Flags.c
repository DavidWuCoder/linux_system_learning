#include <stdio.h>

#define ONE_FLAG (1 << 0)
#define TWO_FLAG (1 << 1)
#define THREE_FLAG (1 << 2)
#define FOUR_FLAG (1 << 3)

void Print(int flags)
{
    if (flags & ONE_FLAG)
    {
        printf("one!\n");
    }
    if (flags & TWO_FLAG)
    {
        printf("two!\n");
    }
    if (flags & THREE_FLAG)
    {
        printf("three!\n");
    }
    if (flags & FOUR_FLAG)
    {
        printf("four!\n");
    }
}

int main()
{
    Print(ONE_FLAG | THREE_FLAG);
    Print(ONE_FLAG | TWO_FLAG | THREE_FLAG | FOUR_FLAG);
    return 0;
}
