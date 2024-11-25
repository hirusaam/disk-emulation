#include "fs-ops.h"

int main()
{
    char str[] = "!-----------------------64 Bytes of Data-----------------------!";
    fs_formatDisk();

    fs_create("f1.txt");
    int fd1 = fs_open("f1.txt");
    fs_create("f2.txt");
    int fd2 = fs_open("f2.txt");

    printf("Write Data: %d\n", fs_write(fd1, str, BLOCKSIZE));
    printf("Write Data: %d\n", fs_write(fd2, str, BLOCKSIZE));
    printf("Seek: %d\n", fs_seek(fd1, BLOCKSIZE));
    printf("Seek: %d\n", fs_seek(fd2, BLOCKSIZE));
    printf("Write Data: %d\n", fs_write(fd1, str, BLOCKSIZE));
    printf("Write Data: %d\n", fs_write(fd2, str, BLOCKSIZE));
    fs_dump();
    fs_delete("f2.txt");
    fs_dump();
    fs_delete("f1.txt");
    fs_dump();
}