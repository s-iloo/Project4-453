#include "libDisk.h"
#include "TinyFSErrors.h"
#include <unistd.h>
#include <fcntl.h>

/*
This functions opens a regular UNIX file and designates the first
nBytes of it as space for the emulated disk. If nBytes is not exactly a
multiple of BLOCKSIZE then the disk size will be the closest multiple
of BLOCKSIZE that is lower than nByte (but greater than 0) If nBytes is
less than BLOCKSIZE failure should be returned. If nBytes > BLOCKSIZE
and there is already a file by the given filename, that file’s content
may be overwritten. If nBytes is 0, an existing disk is opened, and the
content must not be overwritten in this function. There is no requirement
to maintain integrity of any file content beyond nBytes. The return value
is negative on failure or a disk number on success.
*/
int openDisk(char *filename, int nBytes) {

    int file;
    int adjusted_nBytes = (nBytes / BLOCKSIZE) * BLOCKSIZE;
    char buff[adjusted_nBytes];
    int i;

    /*int accesss = access(filename, F_OK);

    printf("doing access check: %d", accesss);
    printf("checking filename: %s", filename);*/

    if ((nBytes == 0) && (access(filename, F_OK) != -1)) {
	file = open(filename, O_RDWR);
        if (file < 0){
            return TFS_DISK_NOT_FOUND;
        }
	return file; 
    } else if (nBytes < BLOCKSIZE) {
        return TFS_ERROR;
    } else if ((file = open(filename, O_RDWR | O_CREAT | O_TRUNC, S_IWGRP | S_IRGRP | S_IWUSR | S_IRUSR)) == -1) {
        return TFS_ERROR;
    }
    // designating first "adjusted_nBytes" of space for the emulated disk
    for (i = 0; i < adjusted_nBytes; i++) {
        buff[i] = 0;    
    }
    if (write(file, buff, adjusted_nBytes) < 0) {
        return TFS_ERROR;
    }
    return file;

}

int closeDisk(int disk) {
    return close(disk);
}

/*
readBlock() reads an entire block of BLOCKSIZE bytes from the open
disk (identified by ‘disk’) and copies the result into a local buffer
(must be at least of BLOCKSIZE bytes). The bNum is a logical block
number, which must be translated into a byte offset within the disk. The
translation from logical to physical block is straightforward: bNum=0
is the very first byte of the file. bNum=1 is BLOCKSIZE bytes into the
disk, bNum=n is n*BLOCKSIZE bytes into the disk. On success, it returns
0. -1 or smaller is returned if disk is not available (hasn’t been
opened) or any other failures. You must define your own error code
system.
*/
int readBlock(int disk, int bNum, void *block) {
    
    // NOTE: Assuming block is the local buffer?
    // TODO condition always true? Above must be off
    if (sizeof(*block) < BLOCKSIZE) {
        return TFS_ERROR;
    }

    if (disk < 0) {
        return TFS_FILE_NOT_OPEN;
    }

    if (bNum < 0) {
        return TFS_INVALID_BLOCK;
    }
 
    if (lseek(disk, bNum * BLOCKSIZE, SEEK_SET) == -1) {
        return TFS_INVALID_SEEK;
    }

    if (read(disk, block, BLOCKSIZE) != BLOCKSIZE) {
        return TFS_ERROR;       
    }
    return TFS_SUCCESS;   

}

/*
writeBlock() takes disk number ‘disk’ and logical block number ‘bNum’
and writes the content of the buffer ‘block’ to that location. ‘block’
must be integral with BLOCKSIZE. Just as in readBlock(), writeBlock()
must translate the logical block bNum to the correct byte position in
the file. On success, it returns 0. -1 or smaller is returned if disk
is not available (i.e. hasn’t been opened) or any other failures. You
must define your own error code system.
*/
int writeBlock(int disk, int bNum, void *block) {
    // TODO condition always true?
    if (sizeof(*block) < BLOCKSIZE) {
        return TFS_ERROR;
    }

    if (disk < 0) {
        return TFS_FILE_NOT_OPEN;
    }

    if (bNum < 0) {
        return TFS_INVALID_BLOCK;
    }

    if (lseek(disk, bNum * BLOCKSIZE, SEEK_SET) == -1) {
        return TFS_INVALID_SEEK;
    }

    if (write(disk, block, BLOCKSIZE) != BLOCKSIZE) {
        return TFS_ERROR;
    }
    return TFS_SUCCESS;
}

