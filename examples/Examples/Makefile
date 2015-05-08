# Makefile for Lab#7
OBJ= timer.o sigsetjmp.o
EXE= timer   sigsetjmp

all : $(EXE)

timer : timer.c
	ccd timer.c

sigsetjmp : sigsetjmp.c
	ccd sigsetjmp.c

clean :
	/bin/rm -f *.o $(EXE)
