# Could be wrong- modeled after previously used Makefiles

# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -g

# Executable name
EXEC = tinyFSDemo

# Object files
OBJS = libDisk.o libTinyFS.o tinyFSDemo.o

# Header files
HEADERS = libDisk.h libTinyFS.h tinyFS_errno.h

# Main demo command: run `make demo`
demo: $(EXEC)

$(EXEC): $(OBJS)
    $(CC) $(CFLAGS) -o $(EXEC) $(OBJS)

libDisk.o: libDisk.c libDisk.h
    $(CC) $(CFLAGS) -c libDisk.c

libTinyFS.o: libTinyFS.c libTinyFS.h libDisk.h tinyFS_errno.h
    $(CC) $(CFLAGS) -c libTinyFS.c

tinyFSDemo.o: tinyFSDemo.c libTinyFS.h libDisk.h tinyFS_errno.h
    $(CC) $(CFLAGS) -c tinyFSDemo.c

clean:
    rm -f $(EXEC) $(OBJS)

