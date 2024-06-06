#ifndef LIBDISK_H
#define LIBDISK_H

#define BLOCKSIZE 256

#include "TinyFS_errno.h"
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

int openDisk(char *filename, int nBytes);
int closeDisk(int disk);
int readBlock(int disk, int bNum, void *block);
int writeBlock(int disk, int bNum, void *block);

#endif
