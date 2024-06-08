#ifndef TINYFS_H
#define TINYFS_H

#define BLOCKSIZE 256
#define DEFAULT_DISK_SIZE 10240
#define DEFAULT_DISK_NAME "tinyFSDisk"

#include "libDisk.h"
#include "TinyFS_errno.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

typedef struct {
    char name[9];
    int size;
    int start_block;
    int curr_block;
    int curr_offset;
    int read_only;
    time_t creation_t;
} fileMetadata;

typedef int fileDescriptor;

/* Standard function declarations */

int tfs_mkfs(char *filename, int nBytes);
int tfs_mount(char *diskname);
int tfs_unmount(void);
fileDescriptor tfs_openFile(char *name);
int tfs_closeFile(fileDescriptor FD);
int tfs_writeFile(fileDescriptor FD, char *buffer, int size);
int tfs_deleteFile(fileDescriptor FD);
int tfs_readByte(fileDescriptor FD, char *buffer);
int tfs_seek(fileDescriptor FD, int offset);

/* Implement file system consistency checks */
int tfs_checkConsistency();

/* Directory Listing and File Renaming */
int tfs_rename(fileDescriptor FD, char* newName);
int tfs_readdir();

/* Read-only and WriteByte Support */
int tfs_makeRO(char *name);
int tfs_makeRW(char *name);
int tfs_writeByte(fileDescriptor FD, int offset, unsigned int data);

/* Timestamps */
int tfs_readFileInfo(fileDescriptor FD);

#endif

