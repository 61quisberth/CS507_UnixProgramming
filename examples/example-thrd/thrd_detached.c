/* thrd_detached.c: illustrates how to detach threads */

/* =====================================================================
 * Header Section: {{{1
 */

/* ---------------------------------------------------------------------
 * Includes: {{{2
 */
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include "xtdio.h"

/* ---------------------------------------------------------------------
 * Prototypes: {{{2
 */
void procA(int *);
void procB(int *);

/* ---------------------------------------------------------------------
 * Global Variables: {{{2
 */
int  resA;
int  resB;

/* =====================================================================
 * Functions: {{{1
 */

/* --------------------------------------------------------------------- */
/* main: {{{2 */
int main(int argc,char **argv)
{
pthread_t      thrdA;
pthread_t      thrdB;
pthread_attr_t attr_detached;

#ifdef PTHREAD_CREATE_DETACHED
printf("main: setting up detached attributes\n");
pthread_attr_init(&attr_detached);
pthread_attr_setdetachstate(&attr_detached,PTHREAD_CREATE_DETACHED);
#endif

/* creating thrdA as detached */
printf("main: creating thrdA\n");
pthread_create(
  &thrdA,			/* pthread_t                        */
  &attr_detached,	/* attribute                        */
  (void *) procA,	/* starting routine for this thread */
  (void *) &resA);	/* argument supplied to this thread */

/* thrdB will be dynamically set to detached later */
printf("main: creating thrdB\n");
pthread_create(
  &thrdB,			/* pthread_t                        */
  NULL,				/* attribute                        */
  (void *) procB,	/* starting routine for this thread */
  (void *) &resB);	/* argument supplied to this thread */

printf("main: is about to sleep for two seconds\n");
sleep(2);

return 0;
}

/* ---------------------------------------------------------- */
/* procA: this thread begins detached {{{2 */
void procA(int *inpA)
{
int i;
int j;
int x;

for(i= 0; i < 4; ++i) {
	printf("procA\n");
	for(j= x= 0; j < 10000; ++j) x= x + i;;
	}
pthread_exit(NULL);
}

/* ---------------------------------------------------------- */
/* procB: {{{2 */
void procB(int *inpA)
{
int i;
int j;
int x;

/* (dynamically) detach this thread */
pthread_detach(pthread_self());

for(i= 0; i < 4; ++i) {
	printf("procB\n");
	for(j= x= 0; j < 10000; ++j) x= x + i;;
	}
pthread_exit(NULL);
}

/* ===================================================================== */
/* Modelines: {{{1
 *  vim: fdm=marker
 */
