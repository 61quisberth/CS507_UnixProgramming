/* thrd2.c: example of thread use
 * Author: Charles Campbell
 * Date: Nov 14, 2008
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

/* ---------------------------------------------------------------------
 * Prototypes: {{{2
 */
void procA(int *);
void procB(int *);
int  resA;
int  resB;

/* =====================================================================
 * Functions: {{{1
 */

/* ---------------------------------------------------------------------
 * main: {{{2
 */
int main(int argc,char **argv)
{
pthread_t thrdA;
pthread_t thrdB;

pthread_create(
  &thrdA,			/* pthread_t                        */
  NULL,				/* attribute                        */
  (void *) procA,	/* starting routine for this thread */
  (void *) &resA);	/* argument supplied to this thread */
pthread_create(
  &thrdB,			/* pthread_t                        */
  NULL,				/* attribute                        */
  (void *) procB,	/* starting routine for this thread */
  (void *) &resB);	/* argument supplied to this thread */

pthread_join(thrdA,NULL);
pthread_join(thrdB,NULL);

return 0;
}

/* ---------------------------------------------------------- */
/* procA: {{{2 */
void procA(int *inpA)
{
int i;
int j;
int x;

printf("procA: starting\n");
for(i= 0; i < 4; ++i) {
	for(j= x= 0; j < 10000; ++j) x= x + i;
	++resA;
	while(resA > resB) {
		printf("procA: waiting while resA[=%d] > resB[=%d]\n",resA,resB);
#if defined(__linux) || defined(sgi)
		pthread_yield();
/*        sched_yield();*/
#else
		thr_yield();
#endif
		sleep(1);
		}
	printf("procA: (loop#%d) resA[=%d] now %s resB[=%d]\n",i,resA,(resA < resB)? "< " : "==",resB);
	}
printf("procA terminating\n");
pthread_exit(NULL);
}

/* ---------------------------------------------------------- */
/* procB: {{{2 */
void procB(int *inpA)
{
int i;
int j;
int x;

printf("procB: starting\n");
for(i= 0; i < 4; ++i) {
	for(j= x= 0; j < 10000; ++j) x= x + i;
	++resB;
	while(resB > resA) {
		printf("procB: waiting while resB[=%d] > resA[=%d]\n",resB,resA);
#if defined(__linux) || defined(sgi)
		pthread_yield();
/*        sched_yield();*/
#else
		thr_yield();
#endif
		sleep(1);
		}
	printf("procB: (loop#%d) resB[=%d] now %s resA[=%d]\n",i,resB,(resB < resA)? "< " : "==",resA);
	}
printf("procB terminating\n");
pthread_exit(NULL);
}

/* ===================================================================== */
/* Modelines: {{{1
 *  vim: fdm=marker
 */
