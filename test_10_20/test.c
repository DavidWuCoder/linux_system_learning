
#include <stdio.h>

int main()
{
	FILE* fp = fopen("/dev/pts/0", "w");
	if (fp == NULL)
	{
		perror("fopen");
		return 1;
	}
	char buffer[1024];
	while(1)
	{
		printf("Enter# ");
	}
}





