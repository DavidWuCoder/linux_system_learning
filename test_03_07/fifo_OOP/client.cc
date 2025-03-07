#include "comm.hpp"

int main()
{
    FileOper writerfile(PATH, FIFO_FILE);

    writerfile.OpenForWrite();

    writerfile.Write();

    writerfile.Close();
    return 0;
}