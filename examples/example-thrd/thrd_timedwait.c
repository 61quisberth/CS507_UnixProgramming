/* thrd_timedwait.c: this program illustrates a sem_timedwait() call
 *   Author: Charles E. Campbell, Jr.
 *   Date:   Apr 21, 2012
 */

/* =====================================================================
 * Header Section: {{{1
 */

/* ---------------------------------------------------------------------
 * Includes: {{{2
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <semaphore.h>
#include "xtdio.h"

/* ------------------------------------------------------------------------
 * Global Data: {{{2
 */
sem_t unnsem;

/* ========================================================================
 * Functions: {{{1
 */

/* --------------------------------------------------------------------- */
/* main: {{{2 */
int main(
  int    argc,
  char **argv)
{
int             ret;
time_t          now;
struct timespec abstime;

Initdbg((&argc,argv));

if(sem_init(&unnsem,0,0)) { /* set unnsem for thread-sharing, value 0 (blocking) */
	perror("sem_init");
	exit(0);
	}

/* set up abstime to two seconds from now */
now             = time(NULL); /* get qty of seconds since Epoch */
abstime.tv_sec  = now + 2L;
abstime.tv_nsec = 0L;

/* block until either the semaphore unnsem becomes available -or-
 * a timeout occurs
 */
if((ret= sem_timedwait(&unnsem,&abstime)) == -1) {
	/* note: this point could also be reached upon receipt of a signal */
	printf("semaphore timeout: ret=%d errno=%d<%s>\n",
	  ret,
	  errno,
	  strerror(errno));
	}
else printf("semaphore started released\n");

Rdbg(("main 0"));
return 0;
}

/* ===================================================================== */
/* Modelines: {{{1
 *  vim: fdm=marker fenc=utf-8 ts=4
 */
