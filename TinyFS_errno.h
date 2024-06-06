#ifndef TINYFS_ERRNO_H
#define TINYFS_ERRNO_H

/* Success Code */
#define TFS_SUCCESS 0

/* Error Codes */
#define TFS_ERROR -1
#define TFS_DISK_NOT_OPEN -2
#define TFS_DISK_NOT_FOUND -3
#define TFS_INVALID_BLOCK -4
#define TFS_DISK_FULL -5
#define TFS_FILE_NOT_FOUND -6
#define TFS_FILE_ALREADY_EXISTS -7
#define TFS_FILE_NOT_OPEN -8
#define TFS_FILE_READ_ONLY -9
#define TFS_INVALID_SEEK -10
#define TFS_WRITE_ERROR -11
#define TFS_READ_ERROR -12
#define TFS_DISK_FAILURE -13

#endif
