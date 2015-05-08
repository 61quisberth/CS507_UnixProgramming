/* tst.c: this program
 *   Author: Charles E. Campbell, Jr.
 *   Date:   Apr 19, 2012
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

/* ========================================================================
 * Functions: {{{1
 */

/* --------------------------------------------------------------------- */
/* main: {{{2 */
int main(
  int    argc,
  char **argv)
{

#ifdef PTHREAD_CREATE_DETACHED
printf("PTHREAD_CREATE_DETACHED\n");
#endif
#ifdef PTHREAD_CREATE_JOINABLE
printf("PTHREAD_CREATE_JOINABLE\n");
#endif
#ifdef _POSIX_THREAD_ATTR_STACKSIZE
printf("_POSIX_THREAD_ATTR_STACKSIZE\n");
#endif
#ifdef _POSIX_THREAD_ATTR_STACKADDR
printf("_POSIX_THREAD_ATTR_STACKADDR\n");
#endif
#ifdef _POSIX_THREAD_ATTR_PRIORITY_SCHEDULING
printf("_POSIX_THREAD_ATTR_PRIORITY_SCHEDULING\n");
#endif
#ifdef _POSIX_THREAD_ATTR_PRIORITY_SCHEDULING
printf("_POSIX_THREAD_ATTR_PRIORITY_SCHEDULING\n");
#endif
#ifdef _POSIX_THREAD_PRIORITY_SCHEDULING
printf("_POSIX_THREAD_PRIORITY_SCHEDULING\n");
#endif
#ifdef PHREAD_SCOPE_SYSTEM
printf("PTHREAD_SCOPE_SYSTEM\n");
#endif
#ifdef PHREAD_SCOPE_PROCESS
printf("PTHREAD_SCOPE_PROCESS\n");
#endif
#ifdef SCHED_FIFO
printf("SCHED_FIFO\n");
#endif
#ifdef SCHED_RR
printf("SCHED_RR\n");
#endif
#ifdef SCHED_OTHER
printf("SCHED_OTHER\n");
#endif
#ifdef PTHREAD_INHERIT_SCHED
printf("PTHREAD_INHERIT_SCHED\n");
#endif
#ifdef PTHREAD_EXPLICIT_SCHED
printf("PTHREAD_EXPLICIT_SCHED\n");
#endif
#ifdef PTHREAD_CANCELED
printf("PTHREAD_CANCELED\n");
#endif
#ifdef PTHREAD_CANCEL_ENABLE
printf("PTHREAD_CANCEL_ENABLE\n");
#endif
#ifdef PTHREAD_CANCEL_DISABLE
printf("PTHREAD_CANCEL_DISABLE\n");
#endif
#ifdef PTHREAD_CANCEL_DEFERRED
printf("PTHREAD_CANCEL_DEFERRED\n");
#endif
#ifdef PTHREAD_CANCEL_ASYNCHRONOUS
printf("PTHREAD_CANCEL_ASYNCHRONOUS\n");
#endif
#ifdef PTHREAD_ONCE_INIT
printf("PTHREAD_ONCE_INIT\n");
#endif
#ifdef PTHREAD_MUTEX_INITIALIZER
printf("PTHREAD_MUTEX_INITIALIZER\n");
#endif
#ifdef _POSIX_THREAD_PRIO_INHERIT
printf("_POSIX_THREAD_PRIO_INHERIT\n");
#endif
#ifdef PTHREAD_COND_INITIALIZER
printf("PTHREAD_COND_INITIALIZER\n");
#endif
#ifdef _POSIX_THREAD_PROCESS_SHARED
printf("_POSIX_THREAD_PROCESS_SHARED=%ld\n",_POSIX_THREAD_PROCESS_SHARED);
#endif
#ifdef PTHREAD_PROCESS_PRIVATE
printf("PTHREAD_PROCESS_PRIVATE\n");
#endif
#ifdef PTHREAD_PROCESS_SHARED
printf("PTHREAD_PROCESS_SHARED\n");
#endif

return 0;
}

/* ===================================================================== */
/* Modelines: {{{1
 *  vim: fdm=marker fenc=utf-8 ts=4
 */
