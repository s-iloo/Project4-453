#include "libTinyFS.h"
#include <stdio.h>
#include "TinyFSErrors.h"


/*
Write a demo program that includes your TinyFS interface to demonstrate the basic
functionality of the required functions and your chosen additional functionality. You can
display informative messages to the screen for the user to see how you demonstrate these.

Main func where we will call our created libraries to demonstrate functionality.
Should run through a majority/ all of tfs_*() functions, with print statements to
trace what is happening throughout. Includes our selected extra features.
*/
int main() {
    // Initialize variables
    char *filename = "tinyFSDisk";
    int nBytes = DEFAULT_DISK_SIZE;
    char writeBuffer[BLOCKSIZE] = "Hello, TinyFS!";
    char readBuffer[BLOCKSIZE];

    // Create a new TinyFS file system
    printf("Creating TinyFS file system...\n");
    int mkfsResult = tfs_mkfs(filename, nBytes);
    if (mkfsResult != TFS_SUCCESS) {
        printf("Error creating TinyFS file system: %d\n", mkfsResult);
        return -1;
    }

    // Mount the TinyFS file system
    printf("Mounting TinyFS file system...\n");
    int mountResult = tfs_mount(filename);
    if (mountResult != TFS_SUCCESS) {
        printf("Error mounting TinyFS file system: %d\n", mountResult);
        return -1;
    }

    // Create a new file
    printf("Creating a new file...\n");
    fileDescriptor fd = tfs_openFile("myfile");
    if (fd.start_block < 0) {
        printf("Error creating file: %d\n", fd);
        return -1;
    }

    // Write data to the file
    printf("Writing data to the file...\n");
    int writeResult = tfs_writeFile(fd, writeBuffer, sizeof(writeBuffer));
    if (writeResult != TFS_SUCCESS) {
        printf("Error writing to file: %d\n", writeResult);
        return -1;
    }

    // Read data from the file
    printf("Reading data from the file...\n");
    int readResult = tfs_readFile(fd, readBuffer, sizeof(readBuffer));
    if (readResult != TFS_SUCCESS) {
        printf("Error reading from file: %d\n", readResult);
        return -1;
    }

    // Display the read data
    printf("Read data: %s\n", readBuffer);

    // Unmount the TinyFS file system
    printf("Unmounting TinyFS file system...\n");
    int unmountResult = tfs_unmount();
    if (unmountResult != TFS_SUCCESS) {
        printf("Error unmounting TinyFS file system: %d\n", unmountResult);
        return -1;
    }

    return 0;
}
