/* thrdcond.c: this program illustrates the use of a condition variable
 * It is based upon an example from the book "Pthreads Programming"
 * by Nichols, Buttlar, and Farrell (ISBN 1-56592-115-1).  I've modified
 * it to include scheduling and priority.
 *
 * Usage: thrdcond [FF [1] | RR [1]]
 *
 * SCHED_RR    : round robin
 * SCHED_FIFO  : first-in first-out -- run a thread 'til it blocks
 * SCHED_OTHER : (implementation dependent) regular non-realtime scheduling
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

/* ------------------------------------------------------------------------
 * Definitions: {{{2
 */
#define TCOUNT      10
#define WATCH_COUNT 12
#if defined(__linux) || defined(sgi)
/*# define ThreadYield()	sched_yield()*/
/*# define ThreadYield()*/
# define ThreadYield()	pthread_yield()
#else
# define ThreadYield()	thr_yield()
#endif

/* ------------------------------------------------------------------------
 * Global Data: {{{2
 */
int             count              = 0;
pthread_mutex_t count_mutex        = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  count_threshold_cv = PTHREAD_COND_INITIALIZER;
int             thread_ids[3]      = {0,1,2};
int             running_ithreads   = 0;

/* ------------------------------------------------------------------------
 * Prototypes: {{{2
 */
int main( int, char **);                                   /* thrdcond.c      */
void watch_count(int *);                                   /* thrdcond.c      */
void inc_count(int *);                                     /* thrdcond.c      */

/* =====================================================================
 * Functions: {{{1
 */

/* --------------------------------------------------------------------- */
/* main: {{{2 */
int main(
  int    argc,
  char **argv)
{
pthread_attr_t    *pisched_attr = NULL;                                          /* "incrementing" thread's scheduling attribute */
pthread_attr_t    *pwsched_attr = NULL;                                          /* "watch" thread's scheduling attribute        */
pthread_attr_t     isched_attr;                                                  /* "incrementing" thread's attribute            */
pthread_attr_t     wsched_attr;                                                  /* "watch" thread's attribute                   */
int                ffmaxprio;
int                ffminprio;
int                i;
pthread_t          threads[3];
struct sched_param iprio;
struct sched_param wprio;

/* set priority */
if(argc > 1) {
	ffmaxprio= sched_get_priority_max(SCHED_FIFO);                               /* get maximum available priority               */
	ffminprio= sched_get_priority_min(SCHED_FIFO);                               /* get minimum available priority               */
	printf("priority may vary from %d to %d\n",ffminprio,ffmaxprio);

	if(!strcmp(argv[1],"FF")) {
		                                                                         /* initialize for FIFO                          */
		pthread_attr_init(&isched_attr);                                         /* intialize "i" attribute                      */
		pthread_attr_init(&wsched_attr);                                         /* initialize "w" attribute                     */
		pisched_attr= &isched_attr;
		pwsched_attr= &wsched_attr;

		                                                                         /* set up FIFO                                  */
		printf("setting up fifo thread attributes\n");
		pthread_attr_setschedpolicy(&isched_attr,SCHED_FIFO);                    /* set "i" to use FIFO                          */
		pthread_attr_setschedpolicy(&wsched_attr,SCHED_FIFO);                    /* set "w" to use FIFO                          */

		                                                                         /* set up priority                              */
		if(argc > 2) {
			printf("setting up fifo thread scheduling\n");
			iprio.sched_priority= ffminprio;                                     /* set "i" thread to use minimum priority       */
			wprio.sched_priority= ffmaxprio;                                     /* set "w" thread to use maximum priority       */
			pthread_attr_setschedparam(&isched_attr,&iprio);                     /* set "i"'s scheduling priority                */
			pthread_attr_setschedparam(&wsched_attr,&wprio);                     /* set "w"'s scheduling priority                */
			}
		}

	else if(!strcmp(argv[1],"RR")) {
		                                                                         /* initialize for round-robin                   */
		pthread_attr_init(&isched_attr);                                         /* initialize "i"'s attribute                   */
		pthread_attr_init(&wsched_attr);                                         /* initialize "i"'s attribute                   */
		pisched_attr= &isched_attr;
		pwsched_attr= &wsched_attr;

		                                                                         /* set up round robin                           */
		printf("setting up round robin thread scheduling\n");
		pthread_attr_setschedpolicy(&isched_attr,SCHED_RR);                      /* set "i" to use round-robin                   */
		pthread_attr_setschedpolicy(&wsched_attr,SCHED_RR);                      /* set "w" to use round-robin                   */

		                                                                         /* set up priority                              */
		if(argc > 2) {
			printf("setting up round-robin thread priorities\n");
			iprio.sched_priority= ffminprio;                                     /* set "i" thread to use minimum priority       */
			wprio.sched_priority= ffmaxprio;                                     /* set "w" thread to use minimum priority       */
			pthread_attr_setschedparam(&isched_attr,&iprio);                     /* set "i"'s scheduling priority                */
			pthread_attr_setschedparam(&wsched_attr,&wprio);                     /* set "w"'s scheduling priority                */
			}
		}
	else {
		printf("using default \"other\" thread scheduling\n");
		}
	}
else {
	printf("thrdcond [FF|RR|other]  for fifo, round-robin, or other\n");
	exit(0);
	}

/* create three threads */
pthread_create(&threads[0], pisched_attr, (void *) inc_count  , &thread_ids[1]); /* create an "i" thread                         */
pthread_create(&threads[1], pisched_attr, (void *) inc_count  , &thread_ids[2]); /* create an "i" thread                         */
pthread_create(&threads[2], pwsched_attr, (void *) watch_count, &thread_ids[0]); /* create a watch thread                        */

/* wait for all three threads to finish */
for(i= 0; i < 3; ++i) {
	printf("main()       : waiting for thread#%d to finish  (running_ithreads=%d)\n",i,running_ithreads);
	pthread_join(threads[i],NULL);
	if(--running_ithreads <= 0) {
		printf("signalling \"watch\" thread we're done\n");
		count= WATCH_COUNT + 1;
		pthread_mutex_lock(&count_mutex);
		pthread_cond_signal(&count_threshold_cv);
		pthread_mutex_unlock(&count_mutex);
		}
	}

return 0;
}

/* --------------------------------------------------------------------- */
/* watch_count: this thread is going to wait on the count-threshold {{{2
 * condition variable.  It first locks the count_mutex.  When the
 * watch_count thread gets signalled, it will print some
 * information and unlock the mutex.
 */
void watch_count(int *idp)
{
printf("watch_count(): thread is about to lock count_mutex  (watch-thread#%d)\n",*idp);
pthread_mutex_lock(&count_mutex);
while(count <= WATCH_COUNT) {
	printf("watch_count(): about to wait upon a condition variable, Count is %d\n",count);
	fflush(stdout);
	pthread_cond_wait(&count_threshold_cv,&count_mutex);
	printf("watch_count(): Thread %d, Count is %d\n",*idp,count);
	}
pthread_mutex_unlock(&count_mutex);
printf("watch_count(): returning from watch-thread#%d\n",*idp);
}

/* --------------------------------------------------------------------- */
/* inc_count: locks the count_mutex, increments count, prints count, {{{2
 * and tests if the threshhold value has been reached.  If it has,
 * inc_count() uses the pthread_cond_signal() function to notify the
 * watch_count() thread of this condition.
 */
void inc_count(int *idp)
{
int i;
int prvcount = 0;

++running_ithreads;

for(i= 0; i < TCOUNT; ++i) {
	pthread_mutex_lock(&count_mutex);
	prvcount= count++;
	if(count == WATCH_COUNT) { /* signal via c.v. every time count reaches WATCH_COUNT */
		printf("inc_count()  : Thread %d, old count %d, newcount %d, signalling via cv\n", *idp,count-1,count);
		pthread_cond_signal(&count_threshold_cv);
		count= 0;
		}
	else printf("inc_count()  : Thread %d, old count %d, newcount %d\n", *idp,prvcount,count);
	pthread_mutex_unlock(&count_mutex);
	ThreadYield();
	}

printf("inc_count()  : returning from inc_count#%d\n",*idp);
}

/* ===================================================================== */
/* Modelines: {{{1
 *  vim: fdm=marker
 */
