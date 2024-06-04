#include "libTinyFS.h"
#include "libDisk.h"
#include "tinyFSErrors.h"
#include <string.h>

static FileDescriptor *file_descriptors = NULL;
static int num_file_descriptors = 0;
static int mounted_disk = -1;

/*
Makes a blank TinyFS file system of size nBytes on the unix file
specified by ‘filename’. This function should use the emulated disk
library to open the specified unix file, and upon success, format the
file to be a mountable disk. This includes initializing all data to 0x00,
setting magic numbers, initializing and writing the superblock and
inodes, etc. Must return a specified success/error code.
*/
int tfs_mkfs(char *filename, int nBytes) {

    int disk = openDisk(filename, nBytes);
    if (disk < 0) {
        return TFS_ERROR;
    }

    char block[BLOCKSIZE] = {0};
    for (int i = 0; i < nBytes / BLOCKSIZE; i++) {
        if (writeBlock(disk, i, block) < 0) {
            return TFS_ERROR;
        }
    }

    // init + write  superblock
    block[0] = 1; // Block type = superblock
    block[1] = 0x44; // Magic number
    // TODO likely more superblock content
    if (writeBlock(disk, 0, block) < 0) {
        return TFS_ERROR;
    }

    // init + write the root inode
    memset(block, 0, BLOCKSIZE);
    block[0] = 2; // Block type = inode
    block[1] = 0x44; // Magic number
    strcpy(block + 4, "root");
    if (writeBlock(disk, 2, block) < 0) {
        return TFS_ERROR;
    }

    closeDisk(disk);
    return TFS_SUCCESS;
}

/*
mounts a TinyFS file system located within given diskname.
As part of the mount operation, tfs_mount should verify the file
system is the correct type. In tinyFS, only one file system may be
mounted at a time. Use tfs_unmount to cleanly unmount the currently
mounted file system. Must return a specified success/error code.
*/
int tfs_mount(char *diskname) {
    if (mounted_disk != -1) {
        return TFS_ERROR;
    }

    int disk = openDisk(diskname, 0);
    if (disk < 0) {
        return TFS_ERROR;
    }

    char block[BLOCKSIZE];
    if (readBlock(disk, 0, block) < 0) {
        return TFS_ERROR;
    }

    if (block[0] != 1 || block[1] != 0x44) {
        closeDisk(disk);
        return TFS_ERROR;
    }

    mounted_disk = disk;
    return TFS_SUCCESS;
}

/*
unmounts the currently mounted file system. Must return a specified success/error code.
*/
int tfs_unmount(void) {
    if (mounted_disk == -1) {
        return TFS_ERROR;
    }

    closeDisk(mounted_disk);
    mounted_disk = -1;
    return TFS_SUCCESS;
}

/*
Creates or Opens a file for reading and writing on the currently
mounted file system. Creates a dynamic resource table entry for the file,
and returns a file descriptor (integer) that can be used to reference
this entry while the filesystem is mounted
*/
fileDescriptor tfs_openFile(char *name) {
    if (mounted_disk == -1) {
        return TFS_ERROR; // No file system is mounted
    }

    // Create a new file descriptor
    FileDescriptor *fd = realloc(file_descriptors, sizeof(FileDescriptor) * (num_file_descriptors + 1));
    if (fd == NULL) {
        return TFS_ERROR;
    }
    file_descriptors = fd;

    // Initialize the file descriptor
    FileDescriptor *new_fd = &file_descriptors[num_file_descriptors];
    strncpy(new_fd->name, name, 8);
    new_fd->name[8] = '\0';
    new_fd->size = 0;
    new_fd->start_block = -1;
    new_fd->current_block = -1;
    new_fd->current_offset = 0;

    num_file_descriptors++;
    return num_file_descriptors - 1; // Return the file descriptor index
}

int tfs_writeFile(fileDescriptor FD, char *buffer, int size) {
    /*
    Writes buffer ‘buffer’ of size ‘size’, which represents an entire
    file’s content, to the file system. Previous content (if any) will be
    completely lost. Sets the file pointer to 0 (the start of file) when
    done. Returns success/error codes.
    */
}

int tfs_deleteFile(fileDescriptor FD) {
    /*
    deletes a file and marks its blocks as free on disk.
    */
}

int tfs_readByte(fileDescriptor FD, char *buffer) {
    /*
    reads one byte from the file and copies it to buffer, using the
    current file pointer location and incrementing it by one upon success.
    If the file pointer is already past the end of the file then
    tfs_readByte() should return an error and not increment the file pointer
    */
}

int tfs_seek(fileDescriptor FD, int offset) {
    /*
    change the file pointer location to offset (absolute). Returns
    success/error codes.
    */
}

/* EXTRA FUNCTIONS. CHECK HEADER FILE FOR MORE INFO ON HOW WE SHOULD APPROACH THESE */

int tfs_checkConsistency() {
     /*
     Perform checks for file system consistency.

     General ideas for consistency checks:
     - Read the superblock and extract information about free blocks and inode pointers
     - Traverse the list of free blocks and ensure they are marked as free
     - traverse the list of inodes and ensure that allocated blocks are not marked as free
     - Check for block corruption (for example: invalid magic numbers, incorrect block types).

     Return TFS_SUCCESS if all checks pass, otherwise return an error code
     */
}

int tfs_rename(fileDescriptor FD, char* newName) {
    /*
    Renames a file. New name should be passed in. File has to be open
    */
}

int tfs_readdir() {
    /*
    Lists all the files and directories on the disk, print the list to stdout
    */
}

int tfs_makeRO(char *name) {
    /*
    makes the file read only. If a file is read only, all tfs_write() and
    tfs_deleteFile() functions that try to use it fail.
    */
}

int tfs_makeRW(char *name) {
    /*
    makes the file read-write
    */
}

int tfs_writeByte(fileDescriptor FD, int offset, unsigned int data) {
    /*
    a function that can write one byte to an exact position inside the file.
    */
}

int tfs_readFileInfo(fileDescriptor FD) {
    /*
    returns the file’s creation time or all info (can be implemented as multiple functions or just this one)
    */
}