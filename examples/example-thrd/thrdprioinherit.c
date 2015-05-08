/* thrdprioinherit.c: this program
 *   Author: Charles E. Campbell
 *   Date:   Apr 05, 2013
 */

/* =====================================================================
 * Header Section: {{{1
 */

/* ---------------------------------------------------------------------
 * Includes: {{{2
 */
#include <stdio.h>
#include "xtdio.h"
#include <pthread.h>
#include <errno.h>

/* ------------------------------------------------------------------------
 * Definitions: {{{2
 */

/* ------------------------------------------------------------------------
 * Typedefs: {{{2
 */

/* ------------------------------------------------------------------------
 * Local Data Structures: {{{2
 */

/* ------------------------------------------------------------------------
 * Global Data: {{{2
 */

/* ------------------------------------------------------------------------
 * Explanation: {{{2
 */

/* ------------------------------------------------------------------------
 * Prototypes: {{{2
 */

/* ========================================================================
 * Functions: {{{1
 */

/* --------------------------------------------------------------------- */
/* main: {{{2 */
int main(
  int    argc,
  char **argv)
{
pthread_mutexattr_t mutexattrA;
pthread_mutex_t     mutexA;
int                 curprotocol;

Initdbg((&argc,argv));
rdcolor();

#ifdef _POSIX_THREAD_PRIO_INHERIT
Dprintf((1,"capability present: _POSIX_THREAD_PRIO_INHERIT\n"));
pthread_mutexattr_init(&mutexattrA);
if(pthread_mutexattr_getprotocol(&mutexattrA,&curprotocol) == -1) {
	printf("unable to getprotocol mutex attribute\n");
	exit(1);
	}
printf("initial protocol=%xx: %s\n",
  curprotocol,
  (curprotocol == _POSIX_THREAD_PRIO_INHERIT)? "prio-inherit" :
  (curprotocol == _POSIX_THREAD_PRIO_PROTECT)? "prio-protect" : "prio-none");

if(curprotocol != _POSIX_THREAD_PRIO_INHERIT) {
	if(pthread_mutexattr_setprotocol(&mutexattrA,_POSIX_THREAD_PRIO_INHERIT) == -1) {
		printf("unable to set mutex protocol to _POSIX_THREAD_PRIO_INHERIT: %s\n",strerror(errno));
		exit(1);
		}
	printf("mutex attribute successfully set to prio-inherit\n");
	if(pthread_mutex_init(&mutexA,&mutexattrA) == -1) {
		printf("unable to initialize mutex: %s\n",strerror(errno));
		exit(1);
		}
	printf("mutex successfully initialized with attribute\n");
	}
#else
printf("cannot do priority inheritance to avoid priority inversion on this system\n");
#endif

Rdbg(("main 0"));
return 0;
}

/* ===================================================================== */
/* Modelines: {{{1
 *  vim: fdm=marker fenc=utf-8 ts=4
 */
