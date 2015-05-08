# Makefile for hw3
#   ILIST is a string containing a list of include files such as "-I/home/cec/.HDR"
#   LLIST is a string containing a list of libraries:  "-L/home/cec/bin/LIB -lncurses -lpthread -lm -lutil"
#   This is using some advanced" gnu-make constructs
#      $(wildcard *.c) expands to a list of all files ending with .c
#      $(SRC:%.c=%) takes the SRC list of *.c files and makes a new list which removes the .c from SRC
#      .c : is a rule that takes .c files to (no suffix) files (ie. builds executables)
#      $< : is the complete target filename (ie. including suffix)
#      $* : is the target filename without its suffix
#      .PHONY: ... gives a list of targets that won't actually be made
SRC=$(wildcard *.c)
EXE=$(SRC:%.c=%)
.PHONY: all

.c :
	@echo compiling $*.c
	@cc -g ${ILIST} -DDEBUG $< ${LLIST} -o $*

all : $(EXE)

clean :
	/bin/rm -f *.o core.* $(EXE) *.out tmp* devranfifo.in
