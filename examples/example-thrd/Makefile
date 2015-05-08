
all : thrd1 thrd2 thrdcond thrd_detached thrdmutex thrdstack thrdstacksize thrd_defines thrdprioinherit thrd_timedwait thrd_unnamedsem thrdpolicy thrdscope thrdprio thrd_rdlock pingpong1 pingpong2 pingpong3

thrd1 : thrd1.c
	cc thrd1 -o thrd1

thrd2 : thrd2.c
	cc thrd2 -o thrd2

thrdcond : thrdcond.c
	cc thrdcond -o thrdcond

thrd_detached : thrd_detached.c
	cc thrd_detached -o thrd_detached

thrdmutex : thrdmutex.c
	cc thrdmutex

thrdstack : thrdstack.c
	cc thrdstack

thrdstacksize : thrdstacksize.c
	cc thrdstacksize

thrd_defines : thrd_defines.c
	cc thrd_defines

thrdprioinherit : thrdprioinherit.c
	cc thrdprioinherit

thrd_timedwait : thrd_timedwait.c
	cc thrd_timedwait

thrd_unnamedsem : thrd_unnamedsem.c
	cc thrd_unnamedsem

thrdscope : thrdscope.c
	cc thrdscope

thrdpolicy : thrdpolicy.c
	cc thrdpolicy

thrdprio : thrdprio.c
	cc thrdprio

thrd_rdlock : thrd_rdlock.c
	cc thrd_rdlock

pingpong1 : pingpong1.c
	cc pingpong1

pingpong2 : pingpong2.c
	cc pingpong2

pingpong3 : pingpong3.c
	cc pingpong3

clean :
	/bin/rm -f *.o thrd[12] core.* thrdcond thrd_detached thrdmutex thrdstack thrdstacksize thrd_defines thrdprioinherit thrd_unnamedsem thrd_timedwait thrdpolicy thrdscope thrdprio thrd_rdlock pingpong1 pingpong2 pingpong3 thrd_rdlock.dat
