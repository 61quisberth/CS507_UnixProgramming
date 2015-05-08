/* thrd_unnamedsem.c : example of unnamed semaphore use
 *   Author: Charles Campbell
 *   Date: Apr 21, 2012
 */

/* =====================================================================
 * Header Section: {{{1
 */

/* ---------------------------------------------------------------------
 * Includes: {{{2
 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

/* ---------------------------------------------------------------------
 * Global Variables: {{{2
 */
sem_t unnsem;

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

/* initialize an unnamed semaphore */
if(sem_init(&unnsem,0,1) == -1) { /* shared amongst threads only, value 1 (unlocked) */
	perror("sem_init");
	exit(0);
	}

/* create two threads */
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

/* wait until two threads rejoin the main thread */
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

for(i= 0; i < 4; ++i) {
	sem_wait(&unnsem); /* succeeds if unnsem's value was >0    */
	printf("procA\n"); /* unnsem's value is now 0              */
	for(j= x= 0; j < 10000; ++j) x= x + i;;
	sem_post(&unnsem); /* unlock semaphore, its value is now 1 */
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

for(i= 0; i < 4; ++i) {
	sem_wait(&unnsem); /* succeeds if unnsem's value was >0    */
	printf("procB\n"); /* unnsem's value is now 0              */
	for(j= x= 0; j < 10000; ++j) x= x + i;;
	sem_post(&unnsem); /* unlock semaphore, its value is now 1 */
	}
pthread_exit(NULL);
}

/* ===================================================================== */
/* Modelines: {{{1
 *  vim: fdm=marker
 */
