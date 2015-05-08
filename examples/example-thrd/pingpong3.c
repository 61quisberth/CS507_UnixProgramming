/* pingpong3.c: this program tests two threads which alternate.  Uses IPC semaphores.
 *   Author: Charles E. Campbell, Jr.
 *   Date:   Apr 26, 2013
 *
 *   Strategy: Uses two IPC semaphores
 *
 *     Ping will wait for semaphore#0 to become positive, and atomically decrements it.
 *     Ping will then do its work and then atomically set semaphore#0 to 0 and semaphore#1 to 1.
 *     This enables Pong.
 *
 *     Pong will wait for semaphore#1 to become positive, and atomically decrements it.
 *     Pong will then do its work and then atomically set semaphore#1 to 0 and semaphore#0 to 1.
 *     This enables Ping.
 */

/* =====================================================================
 * Header Section: {{{1
 */

/* ---------------------------------------------------------------------
 * Includes: {{{2
 */
#include <stdio.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include "xtdio.h"
#include "xmath.h"

/* ------------------------------------------------------------------------
 * Definitions: {{{2
 */
#define	LOOPS 10

/* ------------------------------------------------------------------------
 * Typedefs: {{{2
 */

/* ------------------------------------------------------------------------
 * Local Data Structures: {{{2
 */
ushort        pingarray[2];
ushort        pongarray[2];
struct sembuf semping;
struct sembuf sempong;

/* ------------------------------------------------------------------------
 * Global Data: {{{2
 */
int   semid;
key_t pingpongkey;
int   loops= LOOPS;

/* ------------------------------------------------------------------------
 * Explanation: {{{2
 */

/* ------------------------------------------------------------------------
 * Prototypes: {{{2
 */
int main( int, char **);                /* pingpong.c */
void Ping(int *);                       /* pingpong.c */
void Pong(int *);                       /* pingpong.c */
int pthread_yield(void);

/* ========================================================================
 * Functions: {{{1
 */

/* --------------------------------------------------------------------- */
/* main: {{{2 */
int main(
  int    argc,
  char **argv)
{
pthread_t          ping;
pthread_t          pong;
int                pingresult;
int                pongresult;

Initdbg((&argc,argv));
rdcolor();

if(argc > 1) sscanf(argv[1],"%d",&loops);

/* set up a pair of IPC semaphores initialized to [0,1] */
pingpongkey= ftok(argv[0],0);
if((semid= semget(pingpongkey,2,IPC_CREAT|IPC_EXCL|0777)) < 0) {
	if(errno == EEXIST) {
		Dprintf((1,"trying to get semaphores without IPC_EXCL this time\n"));
		if((semid= semget(pingpongkey,2,IPC_CREAT|0777)) < 0) {
			perror("pingpong3");
			exit(1);
			}
		}
	else {
		perror("pingpong3");
		exit(1);
		}
	}
Dprintf((1,"got semaphores: pingpongkey=%x  semid=%d\n",pingpongkey,semid));

pingarray[0] = 1;                          /* semaphore#0 can be decremented    */
pingarray[1] = 0;                          /* semaphore#1 cannot be decremented */
pongarray[0] = 0;                          /* semaphore#0 cannot be decremented */
pongarray[1] = 1;                          /* semaphore#1 can be decremented    */
if(semctl(semid,0,SETALL,pingarray) < 0) { /* initialize both semaphores        */
	perror("tried to init semaphores");
	semctl(semid,0,IPC_RMID);              /* remove semaphores                 */
	exit(1);
	}

/* set up ping semaphore commands */
semping.sem_num = 0;  /* ping command: semaphore#0        */
semping.sem_op  = -1; /* attempt to decrement semaphore#0 */
semping.sem_flg = 0;  /* block if it can't                */

/* set up pong semaphore commands */
sempong.sem_num = 1;  /* pong command: semaphore#1        */
sempong.sem_op  = -1; /* attempt to decrement semaphore#1 */
sempong.sem_flg = 0;  /* block if it can't                */

pthread_create(&ping,NULL,(void *) Ping,&pingresult);
pthread_create(&pong,NULL,(void *) Pong,&pongresult);
pthread_yield();

pthread_join(ping,NULL);
pthread_join(pong,NULL);

printf("%sping result=%d\n",NRML,pingresult);
printf("%spong result=%d\n",NRML,pongresult);

semctl(semid,0,IPC_RMID); /* remove semaphores */

Rdbg(("main 0"));
return 0;
}

/* --------------------------------------------------------------------- */
/* Ping: this function {{{2 */
void Ping(int *result)
{
int                loop;
int                i;
double             x;

Edbg(("Ping(%sresult)",result? "" : "null "));

for(loop= 0; loop < loops; ++loop) {
	semop(semid,&semping,(size_t)1);
	Dprintf((2,"%sping  : awake\n",GREEN));
	for(i= 0, x= 0.; i < 10000; ++i) {
		x+= BesselJ(0.,((double) i)/10.);
		}
	printf("%sping#%2d: x=%le\n",GREEN,loop,x);
	Dprintf((1,"%sping#%2d: x=%le\n",GREEN,loop,x));
	semctl(semid,0,SETALL,pongarray); /* enable pong */
	}
printf("%sping now trying to exit\n",YELLOW);
fflush(stdout);
fflush(stdout);

*result= (int) x;
fflush(stdout);

Rdbg(("Ping %d",x));
pthread_yield();
pthread_exit(NULL);
}

/* --------------------------------------------------------------------- */
/* Pong: this function {{{2 */
void Pong(int *result)
{
int    loop;
int    i;
double x;

Edbg(("Pong(%sresult)",result? "" : "null "));

for(loop= 0; loop < loops; ++loop) {
	semop(semid,&sempong,(size_t)1);
	Dprintf((2,"%spong  : awake\n",CYAN));
	for(i= 0, x= 0.; i < 10000; ++i) {
		x+= BesselJ(0.,((double) i)/10.);
		}
	printf("%spong#%2d: x=%le\n",CYAN,loop,x);
	Dprintf((1,"%spong#%2d: x=%le\n",CYAN,loop,x));
	semctl(semid,0,SETALL,pingarray); /* enable ping */
	}
printf("%spong now trying to exit\n",YELLOW);
fflush(stdout);
fflush(stdout);

*result= (int) x;
fflush(stdout);

Rdbg(("Pong %d",x));
pthread_yield();
pthread_exit(NULL);
}

/* --------------------------------------------------------------------- */
/* Modelines: {{{1
 * vim: fdm=marker
 */
