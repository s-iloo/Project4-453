Ian Loo, Seamus O'malley, Garrett Green

Features & Functionality

    Basic Functionality 
        Disk Emulation: Implemented using libDisk.c, allowing for creating, reading, and writing to a simulated disk.
        File System Creation: tfs_mkfs creates a new TinyFS file system, formatting it to be mountable.
        Mounting and Unmounting: tfs_mount and tfs_unmount manage mounting and unmounting the file system.
        File Operations: Includes tfs_openFile, tfs_closeFile, tfs_writeFile, tfs_readByte, and tfs_seek for basic file operations.
            All file operations produce correct output and can be seen through our demo program

    Additional Functionality
        Read-Only and WriteByte Support:
            tfs_makeRO: Makes a file read-only. All tfs_write and tfs_deleteFile operations on this file will fail.
            tfs_makeRW: Reverts a read-only file back to read-write.
            tfs_writeByte: Allows writing a single byte to an exact position inside the file (not implemented yet)
        Directory Listing and File Renaming:
            tfs_rename: Renames a file. The file must be open to be renamed.
            tfs_readdir: Lists all files in the file system and prints them to stdout.

        We are able to show this extended functionality in our TinyFSDemo.c program by creating a file, writing to the file, renaming the file 
        while it is open, and converting a file to read-only (and vice versa). We also call tfs_readdir at times in the demo to show a list of 
        files in the system. 

Bugs & Limitations
    TBD 
    magic number being outputted in tfs_readByte