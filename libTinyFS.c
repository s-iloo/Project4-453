#include "libTinyFS.h"
#include "libDisk.h"
#include "tinyFSErrors.h"

int tfs_mkfs(char *filename, int nBytes) {
    /*
    Makes a blank TinyFS file system of size nBytes on the unix file
    specified by ‘filename’. This function should use the emulated disk
    library to open the specified unix file, and upon success, format the
    file to be a mountable disk. This includes initializing all data to 0x00,
    setting magic numbers, initializing and writing the superblock and
    inodes, etc. Must return a specified success/error code.
    */
}

int tfs_mount(char *diskname) {
    /*
    mounts a TinyFS file system located within given diskname.
    As part of the mount operation, tfs_mount should verify the file
    system is the correct type. In tinyFS, only one file system may be
    mounted at a time. Use tfs_unmount to cleanly unmount the currently
    mounted file system. Must return a specified success/error code.
    */
}

int tfs_unmount(void) {
    /*
    unmounts the currently mounted file system. Must return a specified success/error code.
    */
}

fileDescriptor tfs_openFile(char *name) {
    /*
    Creates or Opens a file for reading and writing on the currently
    mounted file system. Creates a dynamic resource table entry for the file,
    and returns a file descriptor (integer) that can be used to reference
    this entry while the filesystem is mounted
    */
}

int tfs_closeFile(fileDescriptor FD) {
    /*
    Closes the file, de-allocates all system resources, and removes table entry
    */
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
