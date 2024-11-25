#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "fs-disk.h"

int fs_create(char *filename);
int fs_open(char *filename);
void fs_delete (char *filename); 
void fs_close(int file_handle);
int fs_read(int file_handle, char *buf, int nbytes);
int fs_write(int file_handle, char *buf, int nbytes);
int fs_seek(int file_handle, int nseek);