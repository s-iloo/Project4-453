#include "libTinyFS.h"

static fileMetadata *file_md = NULL;
static int num_fd = 0;
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
        return TFS_DISK_FAILURE;
    }

    int num_blocks = nBytes / BLOCKSIZE;
    char block[BLOCKSIZE] = {0};

    // Initialize superblock
    block[0] = 1; // Block type = superblock
    block[1] = 0x44; // Magic number
    block[2] = 1; // Pointer to first free block

    if (writeBlock(disk, 0, block) < 0) {
        return TFS_WRITE_ERROR;
    }

    // Initialize free blocks
    int i;
    for (i = 1; i < num_blocks; i++) {
        memset(block, 0, BLOCKSIZE);
        block[0] = 4; // Block type = free
        block[1] = 0x44; // Magic number
        block[2] = (i == num_blocks - 1) ? 0 : i + 1; // Link to the next free block or 0 if the last block

        if (writeBlock(disk, i, block) < 0) {
            return TFS_WRITE_ERROR;
        }
        printf("init free block %d points to: %d\n", i, block[2]);
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
        return TFS_DISK_ALREADY_MOUNTED;
    }

    int disk = openDisk(diskname, 0);
    if (disk < 0) {
        return TFS_DISK_FAILURE;
    }

    char block[BLOCKSIZE];
    if (readBlock(disk, 0, block) < 0) {
        return TFS_READ_ERROR;
    }

    if (block[0] != 1 || block[1] != 0x44) {
        closeDisk(disk);
        return TFS_INVALID_FILESYSTEM;
    }

    mounted_disk = disk;
    return TFS_SUCCESS;
}

/*
unmounts the currently mounted file system. Must return a specified success/error code.
*/
int tfs_unmount(void) {
    if (mounted_disk == -1) {
        return TFS_DISK_NOT_OPEN;
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
        return TFS_DISK_NOT_OPEN;
    }

    int i;
    for (i = 0; i < num_fd; i++) {
        if (strcmp(file_md[i].name, name) == 0) {
            return i; // File already exists, return its descriptor
        }
    }

    fileMetadata *meta = realloc(file_md, sizeof(fileMetadata) * (num_fd + 1));
    if (meta == NULL) {
        return TFS_MEMORY_ERROR;
    }
    file_md = meta;

    fileMetadata *new_meta = &file_md[num_fd];
    strncpy(new_meta->name, name, 8);
    new_meta->name[8] = '\0';
    new_meta->size = 0;
    new_meta->start_block = -1;
    new_meta->curr_block = -1;
    new_meta->curr_offset = 0;
    new_meta->read_only = 0;

    num_fd++;
    return num_fd - 1;
}

/*
Closes the file, de-allocates all system resources, and removes table entry.
*/
int tfs_closeFile(fileDescriptor FD) {
    if (mounted_disk == -1) {
        return TFS_DISK_NOT_OPEN;
    }

    if (FD < 0 || FD >= num_fd) {
        return TFS_FILE_NOT_OPEN;
    }

    // Shift all file descriptors after FD one position left to remove FD
    int i;
    for (i = FD; i < num_fd - 1; i++) {
        file_md[i] = file_md[i + 1];
    }

    num_fd--;

    // shrink the array
    fileMetadata *meta = realloc(file_md, sizeof(fileMetadata) * num_fd);
    if (meta != NULL || num_fd == 0) {
        file_md = meta;
    }

    return TFS_SUCCESS;
}


/*
 * Helper function for writeFile to find free block in disk memory
 */
int find_free_block() {
    char super_block[BLOCKSIZE];
    if (readBlock(mounted_disk, 0, super_block) < 0) {
        return TFS_READ_ERROR;
    }

    int free_block = super_block[2];
    if (free_block == 0) {
        return TFS_DISK_FULL; // No free blocks available
    }

    printf("free block is %d\n", free_block);

    char free_block_data[BLOCKSIZE];
    if (readBlock(mounted_disk, free_block, free_block_data) < 0) {
        return TFS_READ_ERROR;
    }

    // Update superblock to point to the next free block
    super_block[2] = free_block_data[2];
    if (writeBlock(mounted_disk, 0, super_block) < 0) {
        return TFS_WRITE_ERROR;
    }

    printf("updated superblock to block %d\n", super_block[2]);

    // Mark the allocated block as used
    free_block_data[0] = 3; // Data block type
    free_block_data[2] = 0; // Clear next free block pointer
    if (writeBlock(mounted_disk, free_block, free_block_data) < 0) {
        return TFS_WRITE_ERROR;
    }

    return free_block;
}


/*
Writes buffer ‘buffer’ of size ‘size’, which represents an entire
file’s content, to the file system. Previous content (if any) will be
completely lost. Sets the file pointer to 0 (the start of file) when
done. Returns success/error codes.
*/
int tfs_writeFile(fileDescriptor FD, char *buffer, int size) {
    if (mounted_disk == -1) {
        return TFS_DISK_NOT_OPEN;
    }

    if (FD < 0 || FD >= num_fd) {
        return TFS_FILE_NOT_OPEN;
    }

    if (file_md[FD].read_only) {
        return TFS_FILE_READ_ONLY;
    }

    int total_blocks = (size + BLOCKSIZE - 5) / (BLOCKSIZE - 4);
    int remaining_size = size;
    char *current_buffer = buffer;
    int previous_block = -1;

    file_md[FD].size = size;
    file_md[FD].start_block = find_free_block();
    if (file_md[FD].start_block == -1) {
        return TFS_DISK_FULL; // No free blocks available
    }

    file_md[FD].curr_block = file_md[FD].start_block;
    int cur_block = file_md[FD].start_block;

    int i;
    for (i = 0; i < total_blocks; i++) {
        if (cur_block == -1) {
            cur_block = find_free_block();
            if (cur_block == -1) {
                return TFS_DISK_FULL; // No free blocks available
            }

            // Link the previous block to the new block
            if (previous_block != -1) {
                char prev_block_data[BLOCKSIZE];
                if (readBlock(mounted_disk, previous_block, prev_block_data) < 0) {
                    return TFS_READ_ERROR;
                }
                *((int *)(prev_block_data + 2)) = cur_block;
                if (writeBlock(mounted_disk, previous_block, prev_block_data) < 0) {
                    return TFS_WRITE_ERROR;
                }
                printf("linked block %d to new block %d\n", previous_block, cur_block);
            }
        }

        // Write the data to the current block
        char block[BLOCKSIZE] = {0};
        block[0] = 3; // Data block type
        block[1] = 0x44; // Magic number
        int bytes_to_write = (remaining_size > (BLOCKSIZE - 4)) ? (BLOCKSIZE - 4) : remaining_size;
        memcpy(block + 4, current_buffer, bytes_to_write);

        if (writeBlock(mounted_disk, cur_block, block) < 0) {
            return TFS_WRITE_ERROR;
        }
        printf("wrote %d bytes to block no. %d\n", bytes_to_write, cur_block);

        current_buffer += bytes_to_write;
        remaining_size -= bytes_to_write;

        // Update previous and current block pointers
        previous_block = cur_block;
        cur_block = -1;
    }

    file_md[FD].curr_offset = 0;

    return TFS_SUCCESS;
}


/*
deletes a file and marks its blocks as free on disk.
*/
int tfs_deleteFile(fileDescriptor FD) {
    if (mounted_disk == -1) {
        return TFS_DISK_NOT_OPEN;
    }

    if (FD < 0 || FD >= num_fd) {
        return TFS_FILE_NOT_OPEN;
    }

    int curr_block = file_md[FD].start_block;

    while (curr_block != -1) {
        char block[BLOCKSIZE];

        if (readBlock(mounted_disk, curr_block, block) < 0) {
            return TFS_READ_ERROR;
        }

        int next_block = *((int*)(block + 2));

        memset(block, 0, BLOCKSIZE);
        block[0] = 4; // Block type = free
        block[1] = 0x44; // Magic number

        if (writeBlock(mounted_disk, curr_block, block) < 0) {
            return TFS_WRITE_ERROR;
        }

        curr_block = next_block;
    }

    int i;
    for (i = FD; i < num_fd - 1; i++) {
        file_md[i] = file_md[i + 1];
    }
    num_fd--;

    return TFS_SUCCESS;
}

/*
reads one byte from the file and copies it to buffer, using the
current file pointer location and incrementing it by one upon success.
If the file pointer is already past the end of the file then
tfs_readByte() should return an error and not increment the file pointer
*/
int tfs_readByte(fileDescriptor FD, char *buffer) {
    if (mounted_disk == -1) {
        return TFS_DISK_NOT_OPEN;
    }

    if (FD < 0 || FD >= num_fd) {
        return TFS_FILE_NOT_OPEN;
    }

    if (file_md[FD].curr_offset >= file_md[FD].size) {
        return TFS_EOF;
    }

    int block_num = file_md[FD].curr_block;
    int offset = file_md[FD].curr_offset % BLOCKSIZE;

    char block[BLOCKSIZE];
    if (readBlock(mounted_disk, block_num, block) < 0) {
        return TFS_READ_ERROR;
    }

    *buffer = block[offset];
    file_md[FD].curr_offset++;

    if (file_md[FD].curr_offset % BLOCKSIZE == 0 && file_md[FD].curr_offset < file_md[FD].size) {
        file_md[FD].curr_block = *((int*)(block + 2));
    }

    return TFS_SUCCESS;
}


/*
change the file pointer location to offset (absolute). Returns
success/error codes.
*/
int tfs_seek(fileDescriptor FD, int offset) {
    if (mounted_disk == -1) {
        return TFS_DISK_NOT_OPEN;
    }

    if (FD < 0 || FD >= num_fd) {
        return TFS_FILE_NOT_OPEN;
    }

    if (offset < 0 || offset >= file_md[FD].size) {
        return TFS_INVALID_SEEK;
    }

    file_md[FD].curr_offset = offset;
    int block_num = file_md[FD].start_block;

    int i;
    for (i = 0; i < offset / (BLOCKSIZE - 4); i++) {
        char block[BLOCKSIZE];
        if (readBlock(mounted_disk, block_num, block) < 0) {
            return TFS_READ_ERROR;
        }
        block_num = *((int*)(block + 2));
    }

    file_md[FD].curr_block = block_num;
    return TFS_SUCCESS;
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

/*
 Lists all the files and directories on the disk, print the list to stdout
*/
int tfs_readdir() {
    if (mounted_disk == -1) {
        return TFS_DISK_NOT_OPEN;
    }

    printf("Files in TinyFS:\n");
    int i;
    for (i = 0; i < num_fd; i++) {
        printf("%s\n", file_md[i].name);
    }

    return TFS_SUCCESS;
}

/*
 makes the file read only. If a file is read only, all tfs_write() and
 tfs_deleteFile() functions that try to use it fail.
*/
int tfs_makeRO(char *name) {
    int i;
    for (i = 0; i < num_fd; i++) {
        if (strcmp(file_md[i].name, name) == 0) {
            file_md[i].read_only = 1;
            return TFS_SUCCESS;
        }
    }
    return TFS_FILE_NOT_FOUND;
}


/*
 makes the file read-write
*/
int tfs_makeRW(char *name) {
    int i;
    for (i = 0; i < num_fd; i++) {
        if (strcmp(file_md[i].name, name) == 0) {
            file_md[i].read_only = 0;
            return TFS_SUCCESS;
        }
    }
    return TFS_FILE_NOT_FOUND;
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
