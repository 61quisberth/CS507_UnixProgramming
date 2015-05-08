all : thrd1 thrd2 thrdcond thrd_detached thrdmutex thrdstack thrdstacksize thrd_defines thrdprioinherit thrd_timedwait thrd_unnamedsem thrdpolicy thrdscope thrdprio thrd_rdlock pingpong1 pingpong2 pingpong3

thrd1 : thrd1.c
	ccd thrd1

thrd2 : thrd2.c
	ccd thrd2

thrdcond : thrdcond.c
	ccd thrdcond

thrd_detached : thrd_detached.c
	ccd thrd_detached

thrdmutex : thrdmutex.c
	ccd thrdmutex

thrdstack : thrdstack.c
	ccd thrdstack

thrdstacksize : thrdstacksize.c
	ccd thrdstacksize

thrd_defines : thrd_defines.c
	ccd thrd_defines

thrdprioinherit : thrdprioinherit.c
	ccd thrdprioinherit

thrd_timedwait : thrd_timedwait.c
	ccd thrd_timedwait

thrd_unnamedsem : thrd_unnamedsem.c
	ccd thrd_unnamedsem

thrdscope : thrdscope.c
	ccd thrdscope

thrdpolicy : thrdpolicy.c
	ccd thrdpolicy

thrdprio : thrdprio.c
	ccd thrdprio

thrd_rdlock : thrd_rdlock.c
	ccd thrd_rdlock

pingpong1 : pingpong1.c
	ccd pingpong1

pingpong2 : pingpong2.c
	ccd pingpong2

pingpong3 : pingpong3.c
	ccd pingpong3

clean :
	/bin/rm -f *.o thrd[12] core.* thrdcond thrd_detached thrdmutex thrdstack thrdstacksize thrd_defines thrdprioinherit thrd_unnamedsem thrd_timedwait thrdpolicy thrdscope thrdprio thrd_rdlock pingpong1 pingpong2 pingpong3 thrd_rdlock.dat
