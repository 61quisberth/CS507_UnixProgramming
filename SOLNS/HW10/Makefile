# Makefile: supports hw08
SRC=$(wildcard *.c)
EXE=$(SRC:%.c=%)
.PHONY: all
ILIST=-I/home/jason/Documents/CS507_UnixProgramming/drchipdbg
LLIST=/home/jason/Documents/CS507_UnixProgramming/drchipdbg/xtdio.a

all : $(EXE)

.c :
	@echo compiling $*.c
	@cc -g ${ILIST} -DDEBUG $< -pthread ${LLIST} -o $*

clean :
	/bin/rm -f *.o core.* $(EXE) $(OBJS) *.out tmp*
