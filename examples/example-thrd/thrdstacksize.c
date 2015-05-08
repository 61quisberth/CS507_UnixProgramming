/* thrdstacksize.c: illustrates use of setting up a minimum stack size */

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
 * Definitions: {{{2
 */
#define NEWSTACKSIZE	100000

/* ---------------------------------------------------------------------
 * Global Data: {{{2
 */
int resA= 0;

/* ---------------------------------------------------------------------
 * Prototypes: {{{2
 */
void procA(int *);


/* =====================================================================
 * Functions: {{{1
 */

/* --------------------------------------------------------------------- */
/* main: {{{2 */
int main(int argc,char **argv)
{
pthread_t      thrdA;
size_t         stacksize_default;
pthread_attr_t stacksize_attr;

#ifdef _POSIX_THREAD_ATTR_STACKSIZE
printf("_POSIX_THREAD_ATTR_STACKSIZE enabled\n");
pthread_attr_init(&stacksize_attr);                               /* declare a custom attribute         */
pthread_attr_getstacksize(&stacksize_attr,&stacksize_default);    /* query system for default stacksize */
printf("default stacksize=%d\n",stacksize_default);

pthread_attr_setstacksize(&stacksize_attr,(size_t) NEWSTACKSIZE); /* set new minimum stacksize          */
pthread_attr_getstacksize(&stacksize_attr,&stacksize_default);    /* query to see if effect took place  */
printf("new min stacksize=%d\n",stacksize_default);
#endif

/* create a thread with the attribute generated above */
pthread_create(
  &thrdA,			/* pthread_t                        */
  &stacksize_attr,	/* attribute                        */
  (void *) procA,	/* starting routine for this thread */
  (void *) &resA);	/* argument supplied to this thread */

pthread_join(thrdA,NULL);

return 0;
}

/* ---------------------------------------------------------- */
/* procA: {{{2 */
void procA(int *inpA)
{
int i;
int j;
int x;

printf("procA\n");
for(i= 0; i < 4; ++i) {
	printf("  #%d: computing x=sum of %d 10,000 times (the hard way to compute 10000*%d)\n",i,i,i);
	for(j= x= 0; j < 10000; ++j) x= x + i;
	}
sleep(1);
pthread_exit(NULL);
}

/* ===================================================================== */
/* Modelines: {{{1
 *  vim: fdm=marker
 */
