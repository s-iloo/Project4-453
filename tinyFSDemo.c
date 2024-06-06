#include "libTinyFS.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "TinyFS_errno.h"

#define DISK_SIZE 10240

int main() {
    char readBuffer;
    char *iamfileContent, *sillyfileContent;
    int iamfileSize = 200;
    int sillyfileSize = 1000;

    char phrase1[] = "I am file. A very good file. ";
    char phrase2[] = "silly file time ";

    fileDescriptor aFD, bFD;

    printf("Creating and mounting the file system...\n");
    if (tfs_mkfs(DEFAULT_DISK_NAME, DISK_SIZE) != TFS_SUCCESS) {
        printf("Failed to create file system\n");
        return -1;
    }

    if (tfs_mount(DEFAULT_DISK_NAME) != TFS_SUCCESS) {
        printf("Failed to mount file system\n");
        return -1;
    }

    iamfileContent = (char *) malloc(iamfileSize * sizeof(char));
    sillyfileContent = (char *) malloc(sillyfileSize * sizeof(char));

    for (int i = 0; i < iamfileSize; i++) {
        iamfileContent[i] = phrase1[i % strlen(phrase1)];
    }
    iamfileContent[iamfileSize - 1] = '\0';

    for (int i = 0; i < sillyfileSize; i++) {
        sillyfileContent[i] = phrase2[i % strlen(phrase2)];
    }
    sillyfileContent[sillyfileSize - 1] = '\0';

    printf("Opening or creating file \"iamfile\"...\n");
    aFD = tfs_openFile("iamfile");
    if (aFD < 0) {
        printf("Failed to open file \"iamfile\"\n");
        return -1;
    }

    printf("Writing to file \"iamfile\"...\n");
    if (tfs_writeFile(aFD, iamfileContent, iamfileSize) != TFS_SUCCESS) {
        printf("Failed to write to file \"iamfile\"\n");
        return -1;
    }

    printf("Reading from file \"iamfile\"...\n");
    if (tfs_seek(aFD, 0) != TFS_SUCCESS) {
        printf("Failed to seek to the beginning of \"iamfile\"\n");
        return -1;
    }

    while (tfs_readByte(aFD, &readBuffer) == TFS_SUCCESS) {
        printf("%c", readBuffer);
    }
    printf("\n");

    printf("Closing file \"iamfile\"...\n");
    if (tfs_closeFile(aFD) != TFS_SUCCESS) {
        printf("Failed to close file \"iamfile\"\n");
        return -1;
    }

    printf("Opening or creating file \"sillyfile\"...\n");
    bFD = tfs_openFile("sillyfile");
    if (bFD < 0) {
        printf("Failed to open file \"sillyfile\"\n");
        return -1;
    }

    printf("Writing to file \"sillyfile\"...\n");
    if (tfs_writeFile(bFD, sillyfileContent, sillyfileSize) != TFS_SUCCESS) {
        printf("Failed to write to file \"sillyfile\"\n");
        return -1;
    }

    printf("Reading from file \"sillyfile\"...\n");
    if (tfs_seek(bFD, 0) != TFS_SUCCESS) {
        printf("Failed to seek to the beginning of \"sillyfile\"\n");
        return -1;
    }

    while (tfs_readByte(bFD, &readBuffer) == TFS_SUCCESS) {
        printf("%c", readBuffer);
    }
    printf("\n");

    printf("Closing file \"sillyfile\"...\n");
    if (tfs_closeFile(bFD) != TFS_SUCCESS) {
        printf("Failed to close file \"sillyfile\"\n");
        return -1;
    }

    printf("Renaming file \"iamfile\" to \"bruhfile\"...\n");
    if (tfs_rename(aFD, "bruhfile") != TFS_SUCCESS) {
        printf("Failed to rename \"iamfile\"\n");
        return -1;
    }

    printf("Listing files in the file system...\n");
    if (tfs_readdir() != TFS_SUCCESS) {
        printf("Failed to list files\n");
        return -1;
    }

    printf("Making file \"bruhfile\" read-only...\n");
    if (tfs_makeRO("bruhfile") != TFS_SUCCESS) {
        printf("Failed to make \"bruhfile\" read-only\n");
        return -1;
    }

    printf("Attempting to write to read-only file \"bruhfile\" (should fail)...\n");
    if (tfs_writeFile(aFD, iamfileContent, iamfileSize) != TFS_ERROR) {
        printf("Unexpectedly succeeded in writing to read-only file \"bruhfile\"\n");
    } else {
        printf("Correctly failed to write to read-only file \"bruhfile\"\n");
    }

    printf("Unmounting the file system...\n");
    if (tfs_unmount() != TFS_SUCCESS) {
        printf("Failed to unmount the file system\n");
        return -1;
    }

    free(iamfileContent);
    free(sillyfileContent);

    printf("TinyFS demo completed successfully!\n");
    return 0;
}

