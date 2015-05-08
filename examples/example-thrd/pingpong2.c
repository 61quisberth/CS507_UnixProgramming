/* pingpong2.c: this program tests two threads which alternate.  Uses Posix semaphores.
 *   Author: Charles E. Campbell, Jr.
 *   Date:   Apr 26, 2013
 *
 *   Strategy: use two Posix semaphores
 *   
 *   Init: ping-semaphore enabled, pong-semaphore disabled
 *
 *   Ping: wait for ping-semaphore to become enabled
 *   Ping: do Ping's work
 *   Ping: init ping-semaphore to disabled state
 *   Ping: enable Pong's semaphore
 *
 *   Pong: wait for pong-semaphore to become enabled
 *   Pong: do Pong's work
 *   Pong: init pong-semaphore to disabled state
 *   Pong: enable Ping's semaphore
 */

/* =====================================================================
 * Header Section: {{{1
 */

/* ---------------------------------------------------------------------
 * Includes: {{{2
 */
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
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
static sem_t pingsem;
static sem_t pongsem;

/* ------------------------------------------------------------------------
 * Global Data: {{{2
 */
int loops= LOOPS;

/* ------------------------------------------------------------------------
 * Explanation: {{{2
 */

/* ------------------------------------------------------------------------
 * Prototypes: {{{2
 */
int main( int, char **); /* pingpong.c */
void Ping(int *);        /* pingpong.c */
void Pong(int *);        /* pingpong.c */

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

/* initialize semaphores */
sem_init(&pingsem,0,1); /* ping will initially be enabled */
sem_init(&pongsem,0,0); /* pong will initially be blocked */

pthread_create(&pong,NULL,(void *) Pong,&pongresult);
pthread_create(&ping,NULL,(void *) Ping,&pingresult);

pthread_join(ping,NULL);
pthread_join(pong,NULL);

printf("%sping result=%d\n",NRML,pingresult);
printf("%spong result=%d\n",NRML,pongresult);

Rdbg(("main 0"));
return 0;
}

/* --------------------------------------------------------------------- */
/* Ping: this function {{{2 */
void Ping(int *result)
{
int    loop;
int    i;
double x;

Edbg(("Ping(%sresult)",result? "" : "null "));

for(loop= 0; loop < loops; ++loop) {
	sem_wait(&pingsem);
	Dprintf((2,"%sping  : awake\n",CYAN));
	for(i= 0, x= 0.; i < 10000; ++i) {
		x+= BesselJ(0.,((double) i)/10.);
		}
	printf("%sping#%2d: x=%le\n",GREEN,loop,x);
	Dprintf((1,"%sping#%2d: x=%le\n",GREEN,loop,x));
	sem_init(&pingsem,0,0); /* disable ping-semaphore */
	sem_post(&pongsem);
	}
printf("%sping now trying to exit\n",YELLOW);
fflush(stdout);
*result= (int) x;

Rdbg(("Ping %d",x));
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
	sem_wait(&pongsem);
	Dprintf((2,"%spong  : awake\n",CYAN));
	for(i= 0, x= 0.; i < 10000; ++i) {
		x+= BesselJ(0.,((double) i)/10.);
		}
	printf("%spong#%2d: x=%le\n",CYAN,loop,x);
	Dprintf((1,"%spong#%2d: x=%le\n",CYAN,loop,x));
	sem_init(&pongsem,0,0); /* disable pong-semaphore */
	sem_post(&pingsem);
	}
printf("%spong now trying to exit\n",YELLOW);
fflush(stdout);
*result= (int) x;

Rdbg(("Pong %d",x));
pthread_exit(NULL);
}

/* --------------------------------------------------------------------- */
/* Modelines: {{{1
 * vim: fdm=marker
 */
