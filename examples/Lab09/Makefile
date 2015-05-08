# Makefile for Lab#9
#   You'll need to change the CFLAGS and LDFLAGS macros to point to wherever you have put your xtdio.h file and library
OBJ=msgctl.o  msgget.o  msgop.o  semctl.o  semget.o  semop.o  shmctl.o  shmget.o  shmop.o  timer.o sigsetjmp.o
EXE=msgctl    msgget    msgop    semctl    semget    semop    shmctl    shmget    shmop    timer   sigsetjmp
CFLAGS=-I/home/cec/.HDR
LDFLAGS=/home/cec/bin/LIB/xtdio.a -lncurses -lpthread -lm -lutil

all : $(EXE)

msgctl : msgctl.c
	cc ${CFLAGS} msgctl.c ${LDFLAGS} -o msgctl

msgget : msgget.c
	cc ${CFLAGS} msgget.c ${LDFLAGS} -o msgget

msgop : msgop.c
	cc ${CFLAGS} msgop.c ${LDFLAGS} -o msgop

semctl : semctl.c
	cc ${CFLAGS} semctl.c ${LDFLAGS} -o semctl

semget : semget.c
	cc ${CFLAGS} semget.c ${LDFLAGS} -o semget

semop : semop.c
	cc ${CFLAGS} semop.c ${LDFLAGS} -o semop

shmctl : shmctl.c
	cc ${CFLAGS} shmctl.c ${LDFLAGS} -o shmctl

shmget : shmget.c
	cc ${CFLAGS} shmget.c ${LDFLAGS} -o shmget

shmop : shmop.c
	cc ${CFLAGS} shmop.c ${LDFLAGS} -o shmop

timer : timer.c
	cc ${CFLAGS} timer.c ${LDFLAGS} -o timer

clean :
	/bin/rm -f *.o $(EXE)
