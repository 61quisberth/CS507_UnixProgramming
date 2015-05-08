# Makefile: supports hw7
SRC=$(wildcard *.c)
EXE=$(SRC:%.c=%)
.PHONY: all

all : $(EXE)

.c :
	@echo compiling $*.c
	@cc -g ${ILIST} -DDEBUG $< -lncurses ${LLIST} -o $*

clean :
	/bin/rm -f *.o core.* $(EXE) $(OBJS) *.out tmp*
