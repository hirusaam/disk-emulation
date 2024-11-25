#include "fs-ops.h"

int main(){
    fs_formatDisk();
    fs_create("fileabc");
    int fd = fs_open("fileabc");
    char str[] = "!-----------------------64 Bytes of Data-----------------------!";
    printf("Write Data: %d\n", fs_write(fd, str, BLOCKSIZE));
    printf("Seek: %d\n", fs_seek(fd, BLOCKSIZE));
    printf("Write Data: %d\n", fs_write(fd, str, BLOCKSIZE));
    printf("Seek: %d\n", fs_seek(fd, BLOCKSIZE));
    printf("Write Data: %d\n", fs_write(fd, str, BLOCKSIZE));
    printf("Seek: %d\n", fs_seek(fd, BLOCKSIZE));
    printf("Write Data: %d\n", fs_write(fd, str, BLOCKSIZE));
    char buf[BLOCKSIZE * MAX_FILE_SIZE + 1];
    buf[BLOCKSIZE * MAX_FILE_SIZE] = '\0';
    printf("Seek: %d\n", fs_seek(fd, -BLOCKSIZE * 3));
    printf("Read Data %d\n", fs_read(fd, buf, BLOCKSIZE * MAX_FILE_SIZE));
    printf("Data: %s\n", buf);
    fs_close(fd);
    fs_dump();
}
