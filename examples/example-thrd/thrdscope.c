/* thrdscope.c: illustrates use of specifying a scope */

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
pthread_t      thrdA;
pthread_attr_t thrdscope_attr;
int            default_scope;

pthread_attr_init(&thrdscope_attr);                       /* declare a custom attribute */

/* get the default scope and display it */
pthread_attr_getscope(&thrdscope_attr,&default_scope);
printf("default scope=%d<%s>\n",
  default_scope,
  (default_scope == PTHREAD_SCOPE_SYSTEM)?  "system"  :
  (default_scope == PTHREAD_SCOPE_PROCESS)? "process" : "???");

/* set up an attribute with a FIFO scheduling scope
 * Note: linux only supports PTHREAD_SCOPE_SYSTEM,
 *       so this attempt to change the scope fails.
 */
pthread_attr_setscope(&thrdscope_attr,PTHREAD_SCOPE_PROCESS);      /* set thread scope to PROCESS  */
pthread_attr_getscope(&thrdscope_attr,&default_scope);
printf("        scope=%d<%s>\n",
  default_scope,
  (default_scope == PTHREAD_SCOPE_SYSTEM)?  "system"  :
  (default_scope == PTHREAD_SCOPE_PROCESS)? "process" : "???");

/* create a new thread with given attribute */
pthread_create(
  &thrdA,			/* pthread_t                        */
  &thrdscope_attr,	/* attribute                        */
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
