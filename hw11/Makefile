CC=gcc
CFLAGS=-Wall -g -pthread
ILIST=-I
LLIST=

all: simple_threads simple_mutex hw11 fileio

simple_threads: simple_threads.c
	$(CC) $(CFLAGS) simple_threads.c -o simple_threads

simple_mutex: simple_mutex.c
	$(CC) $(CFLAGS) simple_mutex.c -o simple_mutex 

hw11: hw11.c
	$(CC) $(CFLAGS) hw11.c -o hw11 

fileio: fileio.c
	$(CC) $(CFLAGS) fileio.c -o fileio 

clean:
	rm -rf simple_threads simple_mutex hw11 fileio core
