# Makefile for Lab06

LIBB = -I /home/jason/Documents/CS507_UnixProgramming/drchipdbg
MYFLAGS= ${LIBB} -DDEBUG -DDEBUG_TEST ${ILIST} 
MYLIBS=${LLIST}
TGTS=child exec_ls fcntlget filelock pipes race waiter

all : $(TGTS)
	echo "Done making all executables"

child : child.c 
	gcc $(MYFLAGS) child.c $(MYLIBS) -o child

exec_ls : exec_ls.c
	gcc $(MYFLAGS) exec_ls.c $(MYLIBS) -o exec_ls

fcntlget : fcntlget.c
	echo done1
	gcc $(MYFLAGS) fcntlget.c $(MYLIBS) -o fcntlget

filelock : filelock.c
	gcc $(MYFLAGS) filelock.c $(MYLIBS) -o filelock

pipes : pipes.c
	gcc $(MYFLAGS) pipes.c $(MYLIBS) -o pipes

race : race.c
	gcc $(MYFLAGS) race.c $(MYLIBS) -o race

waiter : waiter.c
	gcc $(MYFLAGS) waiter.c $(MYLIBS) -o waiter

clean :
	/bin/rm -f $(TGTS) *.o core*.* tmp*
