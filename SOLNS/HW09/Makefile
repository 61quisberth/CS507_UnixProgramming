ILIST=-I/home/jason/Documents/CS507_UnixProgramming/drchipdbg
LLIST=/home/jason/Documents/CS507_UnixProgramming/drchipdbg/xtdio.a
memmapA : memmapA.c
	@/bin/rm -f memmap[AB] *.o core.* tmp* memmapfile
	cc -g ${ILIST} -DDEBUG  -pthread memmapA.c ${LLIST} -o memmapA
	ln -s memmapA memmapB

clean :
	/bin/rm -f memmap[AB] *.o core.* tmp* memmapfile
