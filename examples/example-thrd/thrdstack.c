/* thrdstackaddr.c: illustrates use of specifying a stack
 *   Note: the pthreads committee has deprecated use of pthread_attr_getstacksize
 *         This program now uses pthread_attr_getstack() instead.
 */

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
unsigned char thread_stack[NEWSTACKSIZE];
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
pthread_t       thrdA;
pthread_attr_t  stackaddr_attr;
size_t         *stackaddr_default;

#ifndef USE_DEPRECATED
size_t          stacksize_default;
#endif

#ifdef _POSIX_THREAD_ATTR_STACKSIZE
printf("_POSIX_THREAD_ATTR_STACKSIZE enabled\n");
pthread_attr_init(&stackaddr_attr);                               /* declare a custom attribute         */
pthread_attr_setstacksize(&stackaddr_attr,(size_t) NEWSTACKSIZE); /* set new minimum stacksize          */

#ifdef _POSIX_THREAD_ATTR_STACKADDR
printf("_POSIX_THREAD_ATTR_STACKADDR enabled\n");
# ifdef USE_DEPRECATED
pthread_attr_getstackaddr(&stackaddr_attr,(void **)&stackaddr_default);
printf("default stack address=%lux\n",stackaddr_default);
# else
pthread_attr_getstack(&stackaddr_attr,(void **) &stackaddr_default,&stacksize_default);
printf("default stack address=%lux\n",stackaddr_default);
printf("        stack size   =%lux\n",stacksize_default);
# endif
# if defined(__linux)
pthread_attr_setstack(&stackaddr_attr,thread_stack,(size_t)NEWSTACKSIZE);
# else
pthread_attr_setstackaddr(&stackaddr_attr,thread_stack);
# endif
# endif
#endif

/* create a new thread with given attribute */
pthread_create(
  &thrdA,			/* pthread_t                        */
  &stackaddr_attr,	/* attribute                        */
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
