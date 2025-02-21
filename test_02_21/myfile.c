
#include <stdio.h>
#include <string.h>

int main()
{
    FILE *fp = fopen("log.txt", "a");
    if (fp == NULL)
    {
        perror("fopen");
        return 1;
    }

    const char *msg = "hello bite!\n";
    fprintf(fp, "%s", msg);

    fclose(fp);

    // printf("hello world\n");

    // fprintf(stdout, "hello fprintf\n");

    // const char *msg = "hello fwrite\n";
    // fwrite(msg, strlen(msg), 1, stdout);

    return 0;
}

// cat myfile.txt
// int main(int argc, char *argv[])
//{
//    if(argc != 2)
//    {
//        printf("Usage: %s filename\n", argv[0]);
//        return 1;
//    }
//    FILE *fp = fopen(argv[1], "r");
//    if(NULL == fp)
//    {
//        perror("fopen");
//        return 2;
//    }
//
//    while(1)
//    {
//        char buffer[128];
//        memset(buffer, 0, sizeof(buffer));
//        int n = fread(buffer, sizeof(buffer)-1, 1, fp);
//        if(n > 0)
//        {
//            printf("%s", buffer);
//        }
//        if(feof(fp))
//            break;
//    }
//
//    fclose(fp);
//    return 0;
//}
//
////int main()
////{
////    FILE *fp = fopen("log.txt", "w");
////    if(fp == NULL)
////    {
////        perror("fopen");
////        return 1;
////    }
////
////    const char *msg = "hello bit: ";
////    int cnt = 1;
////    while(cnt <= 10)
////    {
////        char buffer[1024];
////        snprintf(buffer, sizeof(buffer), "%s%d\n", msg, cnt++);
////        fwrite(buffer, strlen(buffer), 1, fp);
////    }
////
////    fclose(fp);
////    return 0;
////}
