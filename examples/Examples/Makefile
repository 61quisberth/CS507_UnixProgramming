ILIST=-I/home/jason/Documents/CS507_UnixProgramming/drchipdbg 
LLIST=/home/jason/Documents/CS507_UnixProgramming/drchipdbg/xtdio.a 

# Makefile for Lab#7

OBJ= timer.o sigsetjmp.o
EXE= timer   sigsetjmp

all : $(EXE)

timer : timer.c
	cc ${ILIST} timer.c ${LLIST} -o timer

sigsetjmp : sigsetjmp.c
	cc ${ILIST} sigsetjmp.c ${LLIST} -o sigsetjmp

clean :
	/bin/rm -f *.o $(EXE)
