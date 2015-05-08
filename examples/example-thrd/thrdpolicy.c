/* thrdpolicy.c: illustrates use of specifying a policy */

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
int           resA                       = 0;

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
pthread_attr_t thrdpolicy_attr;
int            default_policy;

pthread_attr_init(&thrdpolicy_attr);                       /* declare a custom attribute */

#ifdef _POSIX_THREAD_PRIORITY_SCHEDULING
/* get the default policy and display it */
pthread_attr_getschedpolicy(&thrdpolicy_attr,&default_policy);
printf("default policy=%d<%s>\n",
  default_policy,
  (default_policy == SCHED_FIFO)?  "fifo"  :
  (default_policy == SCHED_RR)?    "rr"    :
  (default_policy == SCHED_OTHER)? "other" : "???");

/* set up an attribute with a FIFO scheduling policy */
pthread_attr_setschedpolicy(&thrdpolicy_attr,SCHED_FIFO);      /* set thread policy to FIFO  */
pthread_attr_getschedpolicy(&thrdpolicy_attr,&default_policy);
printf("        policy=%d<%s>\n",
  default_policy,
  (default_policy == SCHED_FIFO)?  "fifo"  :
  (default_policy == SCHED_RR)?    "rr"    :
  (default_policy == SCHED_OTHER)? "other" : "???");
#endif

/* create a new thread with given attribute */
pthread_create(
  &thrdA,			/* pthread_t                        */
  &thrdpolicy_attr,	/* attribute                        */
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
