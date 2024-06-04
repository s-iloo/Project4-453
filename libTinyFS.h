#ifndef TINYFS_H
#define TINYFS_H

#define BLOCKSIZE 256
#define DEFAULT_DISK_SIZE 10240
#define DEFAULT_DISK_NAME "tinyFSDisk"

#include "libDisk.h"
#include "tinyFSErrors.h"
#include <string.h>
#include <stdlib.h>

typedef struct {
    char name[9];
    int size;
    int start_block;
    int current_block;
    int current_offset;
} fileDescriptor;

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

/*
Each below group of api methods worth 10 percent
on top of 80 percent from impl base project.
Need to do 2 of these to get full credit.
Can do all 4 to get up to 120% on project.

Start with the easier ones (obviously after implementing all the above).
They are in no p[articular order but picked the 4 that seemed most feasible
from reading instructions. Can swap them as well if one or multiple
end up being really hard or not straightforward.

Would prefer 2 very solid implementations over 4 mediocre ones.
*/

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
