/* sigsender.c: this program
 *   Author: Charles E. Campbell
 *   Date:   Apr 17, 2011
 */

/* =====================================================================
 * Header Section: {{{1
 */

/* ---------------------------------------------------------------------
 * Includes: {{{2
 */
#include <stdio.h>
#include <sys/types.h>
#include <signal.h>
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
int isig;
int pid;

Initdbg((&argc,argv));
rdcolor();

if(argc <= 1) {
	printf("usage: sigsender pid\n");
	exit(-1);
	}
sscanf(argv[1],"%d",&pid);
for(isig= 1; isig <= 31; ++isig) {
	if(isig == SIGKILL) continue;
	if(isig == 19)      continue;
	kill(pid,isig);
	}

Rdbg(("main 0"));
return 0;
}

/* ===================================================================== */
/* Modelines: {{{1
 *  vim: fdm=marker fenc=utf-8 ts=4
 */
