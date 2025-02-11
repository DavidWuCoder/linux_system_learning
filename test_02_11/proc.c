#include <errno.h>
#include <stdio.h>
#include <string.h>

int main() {
  // printf("hello world\n");

  // FILE *fp = fopen("log.txt", "r");
  // if (fp == NULL) {
  //   return errno;
  // }
  // // 读取
  // fclose(fp);

  // int i = 0;
  // for (int i = 0; i < 200; i++) {
  //   printf("%d -> %s \n", i, strerror(i));
  // }

  // return errno;
  //

  int a = 10;
  a /= 0;
  return 89;
}
