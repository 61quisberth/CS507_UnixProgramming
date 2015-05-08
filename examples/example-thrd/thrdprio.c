/* thrdprio.c: this program
 *   Author: Charles E. Campbell
 *   Date:   Apr 18, 2013
 */

/* =====================================================================
 * Header Section: {{{1
 */

/* ---------------------------------------------------------------------
 * Includes: {{{2
 */
#include <stdio.h>
#include <pthread.h>
#include <errno.h>
#include "xtdio.h"

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
Initdbg((&argc,argv));
rdcolor();

printf("fifo        priorities range from: %d to %d\n",
  sched_get_priority_min(SCHED_FIFO),
  sched_get_priority_max(SCHED_FIFO));

printf("round-robin priorities range from: %d to %d\n",
  sched_get_priority_min(SCHED_RR),
  sched_get_priority_max(SCHED_RR));

printf("other       priorities range from: %d to %d\n",
  sched_get_priority_min(SCHED_OTHER),
  sched_get_priority_max(SCHED_OTHER));

Rdbg(("main 0"));
return 0;
}

/* ===================================================================== */
/* Modelines: {{{1
 *  vim: fdm=marker fenc=utf-8 ts=4
 */
