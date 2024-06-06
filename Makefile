CC = gcc
CFLAGS = -Wall -g

all: tinyFSDemo

tinyFSDemo: libDisk.o libTinyFS.o tinyFSDemo.o
	$(CC) $(CFLAGS) -o tinyFSDemo libDisk.o libTinyFS.o tinyFSDemo.o

libDisk.o: libDisk.c libDisk.h
	$(CC) $(CFLAGS) -c libDisk.c

libTinyFS.o: libTinyFS.c libTinyFS.h TinyFS_errno.h
	$(CC) $(CFLAGS) -c libTinyFS.c

tinyFSDemo.o: tinyFSDemo.c libTinyFS.h
	$(CC) $(CFLAGS) -c tinyFSDemo.c

clean:
	rm -f *.o tinyFSDemo

rm disk:
	rm -f *.dsk
