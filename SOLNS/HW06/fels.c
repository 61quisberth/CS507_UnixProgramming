/* fels.c: this program implements hw06 Q2
 *   Author: Charles E. Campbell
 *   Date:   Mar 05, 2011
 */

/* =====================================================================
 * Header Section: {{{1
 */

/* ---------------------------------------------------------------------
 * Includes: {{{2
 */
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
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
int pid;

Initdbg((&argc,argv));
rdcolor();

if((pid= fork()) > 0 ) { /* parent process */
	int status;
	Dprintf((1,"parent process\n"));
	waitpid(pid,&status,0);
	printf("WIFEXITED   = %d\n",WIFEXITED(status));
	printf("WEXITSTATUS = %d\n",WEXITSTATUS(status));
	printf("WIFSIGNALED = %d\n",WIFSIGNALED(status));
	printf("WTERMSIG    = %d\n",WTERMSIG(status));
	printf("WIFSTOPPED  = %d\n",WIFSTOPPED(status));
	printf("WSTOPSIG    = %d\n",WSTOPSIG(status));
	}
else if(pid == 0) {      /* child process  */
	Dprintf((1,"child process\n"));
	if(execl("/bin/ls","/bin/ls",NULL) == -1) {
		printf("execl(ls): %s\n",strerror(errno));
		exit(-1);
		}
	exit(0);
	}
else {                   /* error          */
	Dprintf((1,"fork failed\n"));
	printf("attempted to fork: %s\n",strerror(errno));
	exit(-1);
	}

Rdbg(("main 0"));
return 0;
}

/* ===================================================================== */
/* Modelines: {{{1
 *  vim: fdm=marker fenc=utf-8 ts=4
 */
