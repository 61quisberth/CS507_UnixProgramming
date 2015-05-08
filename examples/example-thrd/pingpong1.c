/* pingpong1.c: this program tests two threads which alternate.  Uses FIFOs, condition variables, and priority.
 *   Author: Charles E. Campbell, Jr.
 *   Date:   Apr 26, 2013
 *
 *   Strategy: use mutexes and condition variables
 *   NOTE: Under Linux, has to be run as root so that the priorities and fifo policy take effect.
 *
 *   Initialize two priorities:
 *     nrmlprio  (normal priority)
 *     specprio  (special priority > normal priority)
 *     Main: initializes Ping to specprio
 *     Main: initializes Pong to specprio
 *     Main: yields (so Ping and Pong start up)
 *     Main: signal Ping-cv
 *
 *     Ping: switch to special priority
 *     Ping: wait for Ping-cv to be signalled (1st one from main, subsequent from Pong)
 *     Ping: switch to nrmlprio
 *     Ping: yield
 *     Ping: do work
 *     Ping: signal Pong-cv
 *
 *     Pong: switch to special priority
 *     Pong: wait for Pong-cv to be signalled
 *     Pong: switch to nrmlprio
 *     Pong: yield
 *     Pong: do work
 *     Pong: signal Ping-cv
 */

/* =====================================================================
 * Header Section: {{{1
 */

/* ---------------------------------------------------------------------
 * Includes: {{{2
 */
#include <stdio.h>
#include <pthread.h>
#include "xtdio.h"
#include "xmath.h"

/* ------------------------------------------------------------------------
 * Definitions: {{{2
 */
#define	LOOPS 10
/*#define NOISY |+ to see policy and priority while running +|*/

/* ------------------------------------------------------------------------
 * Typedefs: {{{2
 */

/* ------------------------------------------------------------------------
 * Local Data Structures: {{{2
 */

/* ------------------------------------------------------------------------
 * Global Data: {{{2
 */
pthread_mutex_t pingmtx  = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t pongmtx  = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  pingcv   = PTHREAD_COND_INITIALIZER;
pthread_cond_t  pongcv   = PTHREAD_COND_INITIALIZER;
int             minprio;  /* minimum priority for FIFOs  */
int             maxprio;  /* maximum priority for FIFOs  */
int             nrmlprio; /* normal priority             */
int             specprio; /* special (elevated) priority */
int             loops    = LOOPS;
struct sched_param nrmlsp;
struct sched_param specsp;

/* ------------------------------------------------------------------------
 * Explanation: {{{2
 */

/* ------------------------------------------------------------------------
 * Prototypes: {{{2
 */
int main( int, char **);             /* pingpong.c */
void Ping(int *);                    /* pingpong.c */
void Pong(int *);                    /* pingpong.c */
static void SchedReport(int,char *); /* pingpong.c */
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
int                pingresult;
int                policy;
int                pongresult;
pthread_attr_t     attr;
pthread_t          ping;
pthread_t          pong;
struct sched_param mainsp;

Initdbg((&argc,argv));
rdcolor();

if(argc > 1) sscanf(argv[1],"%d",&loops);

/* determine normal and special priority */
minprio               = sched_get_priority_min(SCHED_FIFO);
maxprio               = sched_get_priority_max(SCHED_FIFO);
nrmlprio              = (minprio + maxprio)/2;
specprio              = nrmlprio + 1;
mainsp.sched_priority = nrmlprio;
nrmlsp.sched_priority = nrmlprio;
specsp.sched_priority = specprio;
if(pthread_setschedparam(pthread_self(),SCHED_FIFO,&mainsp) < 0) {
	perror("main setschedparam");
	exit(1);
	}
Dprintf((1,"nrmlprio=%d specprio=%d\n",nrmlprio,specprio));
SchedReport(0,"main");

pthread_attr_init(&attr);
pthread_attr_setschedpolicy(&attr,SCHED_FIFO); /* initialize new threads with FIFO             */
pthread_attr_setschedparam(&attr,&specsp);     /* initialize new threads with special priority */
#ifdef DEBUG
pthread_attr_getschedpolicy(&attr,&policy);
Dprintf((1,"attribute's scheduling policy=%d<%s>\n",
  policy,
  (policy == SCHED_FIFO)?  "fifo"  :
  (policy == SCHED_RR)?    "rr"    :
  (policy == SCHED_OTHER)? "other" : "???"));
#endif

pthread_create(&ping,&attr,(void *) Ping,&pingresult);
pthread_create(&pong,&attr,(void *) Pong,&pongresult);
pthread_yield();

Dprintf((1,"main: signalling ping's c.v.\n"));
printf("main: signalling ping's c.v.\n");
pthread_cond_signal(&pingcv);
Dprintf((1,"main: waiting for ping to join\n"));
pthread_join(ping,NULL);
Dprintf((1,"main: waiting for pong to join\n"));
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
int                loop;
int                i;
double             x;

Edbg(("Ping(%sresult)",result? "" : "null "));
SchedReport(-1,"ping");

for(loop= 0; loop < loops; ++loop) {
	Dprintf((2,"ping#%2d: setting spec priority\n",loop));
	pthread_setschedparam(pthread_self(),SCHED_FIFO,&specsp);
	Dprintf((2,"ping#%2d: locking pingmtx\n",loop));
	pthread_mutex_lock(&pingmtx);
	Dprintf((2,"ping#%2d: waiting on cv\n",loop));
	pthread_cond_wait(&pingcv,&pingmtx);
	Dprintf((2,"ping#%2d: unlocking pingmtx\n",loop));
	pthread_mutex_unlock(&pingmtx);
	Dprintf((2,"ping#%2d: setting nrml priority\n",loop));
	pthread_setschedparam(pthread_self(),SCHED_FIFO,&nrmlsp);
	pthread_yield();
	Dprintf((2,"ping#%2d: awake\n",loop));
	for(i= 0, x= 0.; i < 10000; ++i) {
		x+= BesselJ(0.,((double) i)/10.);
		}
	Dprintf((1,"ping#%2d: x=%le\n",loop,x));
	printf("%sping#%2d: x=%le\n",GREEN,loop,x);
	SchedReport(loop,"ping");
	if(pthread_setschedparam(pthread_self(),SCHED_FIFO,&specsp) < 0) {
		perror("ping setschedparam");
		exit(1);
		}
	Dprintf((1,"ping  : signalling pong's cv\n"));
	pthread_cond_signal(&pongcv);
	}
printf("%sping now trying to exit\n",YELLOW);
pthread_cond_signal(&pongcv);
*result= (int) x;
fflush(stdout);
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
SchedReport(-1,"pong");

for(loop= 0; loop < loops; ++loop) {
	Dprintf((2,"pong#%2d: setting spec priority\n",loop));
	pthread_setschedparam(pthread_self(),SCHED_FIFO,&specsp);
	Dprintf((2,"pong#%2d: locking pongmtx\n",loop));
	pthread_mutex_lock(&pongmtx);
	Dprintf((2,"pong#%2d: waiting on cv\n",loop));
	pthread_cond_wait(&pongcv,&pongmtx);
	Dprintf((2,"pong#%2d: unlocking pongmtx\n",loop));
	pthread_mutex_unlock(&pongmtx);
	Dprintf((2,"pong#%2d: setting nrml priority\n",loop));
	pthread_setschedparam(pthread_self(),SCHED_FIFO,&nrmlsp);
	pthread_yield();
	Dprintf((2,"pong#%2d: awake\n",loop));
	for(i= 0, x= 0.; i < 10000; ++i) {
		x+= BesselJ(0.,((double) i)/10.);
		}
	Dprintf((1,"pong#%2d: x=%le\n",loop,x));
	printf("%spong#%2d: x=%le\n",CYAN,loop,x);
	SchedReport(loop,"pong");
	Dprintf((1,"pong#%2d: signalling ping's cv\n",loop));
	pthread_cond_signal(&pingcv);
	}
printf("%spong now trying to exit\n",YELLOW);
pthread_cond_signal(&pingcv);
*result= (int) x;
fflush(stdout);
pthread_exit(NULL);
}

/* --------------------------------------------------------------------- */
/* SchedReport: this function unfortunately demonstrates that the process is not allowed to change scheduling or priority {{{2 */
static void SchedReport(int loop,char *thrdname)
{
struct sched_param sp;
int                policy;

pthread_getschedparam(pthread_self(),&policy,&sp);
#ifdef NOISY
printf("%s#%2d: policy=%d<%s> priority=%d\n",
  thrdname,
  loop,
  policy,
  (policy == SCHED_RR)?    "rr"    :
  (policy == SCHED_FIFO)?  "fifo"  :
  (policy == SCHED_OTHER)? "other" : "???",
  sp.sched_priority);
#endif
Dprintf((1,"%s#%2d: policy=%d<%s> priority=%d\n",
  thrdname,
  loop,
  policy,
  (policy == SCHED_RR)?    "rr"    :
  (policy == SCHED_FIFO)?  "fifo"  :
  (policy == SCHED_OTHER)? "other" : "???",
  sp.sched_priority));
if(sp.sched_priority < nrmlprio) {
	fprintf(stderr,"%s***error***%s you need to run as root to get FIFO and elevated thread priorities\n",CYAN,NRML);
	exit(1);
	}
}

/* --------------------------------------------------------------------- */
/* Modelines: {{{1
 * vim: fdm=marker
 */
