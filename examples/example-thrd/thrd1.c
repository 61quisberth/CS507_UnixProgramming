/* thrd1.c : example of thread use
 *   Author: Charles Campbell
 *   Date: Nov 14, 2008
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

for(i= 0; i < 4; ++i) {
	printf("procA\n");
	for(j= x= 0; j < 10000; ++j) x= x + i;
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
	printf("procB\n");
	for(j= x= 0; j < 10000; ++j) x= x + i;
	}
pthread_exit(NULL);
}

/* ===================================================================== */
/* Modelines: {{{1
 *  vim: fdm=marker
 */
