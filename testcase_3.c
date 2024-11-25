#include "fs-ops.h"

int main()
{
    fs_formatDisk();
    fs_create("fileabc");
    int fd = fs_open("fileabc");
    fs_close(fd);
    fs_dump();
    fs_delete("fileabc");
    fs_dump();
}
