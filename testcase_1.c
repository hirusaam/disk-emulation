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
  char buf[BLOCKSIZE * 2 + 1];
  buf[BLOCKSIZE * 2] = '\0';
  printf("Seek: %d\n", fs_seek(fd, -BLOCKSIZE * 2));
  printf("Read Data %d\n", fs_read(fd, buf, BLOCKSIZE * 2));
  printf("Data: %s\n", buf);
  fs_close(fd);
  fs_dump();
}
