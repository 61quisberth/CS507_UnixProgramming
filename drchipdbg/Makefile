# Makefile for example using DrChip's inline debugger
#  This example is set up for Unix
SRC = cprt.c   error.c    outofmem.c srmtrblk.c stpnxt.c \
      dbg.c    sprt.c     stpblk.c   xvar.c     rdcolor.c fopenv.c stpbrk.c strprintf.c
OBJ = cprt.o   error.o    outofmem.o srmtrblk.o stpnxt.o \
      dbg.o    sprt.o     stpblk.o   xvar.o     rdcolor.o fopenv.o stpbrk.o strprintf.o

example : example.o xtdio.a
	cc example.o xtdio.a -o example -lpthread

example.o : example.c dbg.h
	cc -c -DDEBUG example.c

xtdio.a : $(OBJ)
	ar r xtdio.a $(OBJ)
	# ranlib xtdio.a    # <-- some Unix systems need ranlib

clean :
	/bin/rm -f *.o xtdio.a tags tmp* example
