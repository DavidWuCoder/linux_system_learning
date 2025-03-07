#include "comm.hpp"


int main()
{
    NamedFifo fifo(PATH, FIFO_FILE);

    // 文件操作
    FileOper readerfile(PATH, FIFO_FILE);

    readerfile.OpenForRead();

    readerfile.Read();

    readerfile.Close();
    return 0;
}