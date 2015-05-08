/* race.c: this program illustrates a fork with two printfs
 */

/* =====================================================================
 * Header Section: {{{1
 */

/* ---------------------------------------------------------------------
 * Includes: {{{2
 */
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include "xtdio.h"

/* =====================================================================
 * Functions: {{{1
 */

/* --------------------------------------------------------------------- */
/* main: {{{2 */
int main(
  int    argc,
  char **argv)
{
pid_t pid;

if((pid= fork()) > 0) { /* parent process */
	pid_t childpid;
	int   status;
	printf("I'm the parent process: pid=%xx  ppid=%xx pgrp=%xx pgid=%xx\n",getpid(),getppid(),getpgrp(),getpgid(getpid()));
	childpid= wait(&status);
	exit(0);
	}
else if(pid == 0) {     /* child process  */
	printf("I'm the child  process: pid=%xx  ppid=%xx pgrp=%xx pgid=%xx\n",getpid(),getppid(),getpgrp(),getpgid(getpid()));
	exit(0);
	}
else error(XTDIO_ERROR,"fork error: <%s>\n",strerror(errno));

return 0;
}

/* ===================================================================== */
/* Modelines: {{{1
 *  vim: fdm=marker fenc=utf-8 ts=4
 */
