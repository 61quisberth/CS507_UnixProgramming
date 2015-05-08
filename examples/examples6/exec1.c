/* exec.c: this program illustrates a use of calling exec
 *   calls fork -- resulting in two processes
 *     the original process is the parent
 *     the new process is the child
 *   parent calls wait -- parent process is blocked until the child terminates
 *   child calls execv and runs /bin/ls with the first argument passed to the parent
 *   The child gets a copy of the parent's argv[1] so it can use it.
 */

/* =====================================================================
 * Header Section: {{{1
 */

/* ---------------------------------------------------------------------
 * Includes: {{{2
 */
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
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

if((pid= fork()) > 0) {
	pid_t childpid;
	int   status;
	/* parent process */
	printf("I'm the parent process: pid=%xx  ppid=%xx pgrp=%xx pgid=%xx\n",getpid(),getppid(),getpgrp(),getpgid(getpid()));
	childpid= wait(&status);
	printf("Well, I'm done waiting on that child!\n");
	}
else if(pid == 0) {
	char *childargv[3];
	/* child process */
	printf("I'm the child  process: pid=%xx  ppid=%xx pgrp=%xx pgid=%xx\n",getpid(),getppid(),getpgrp(),getpgid(getpid()));
	childargv[0]= "/bin/ls";
	childargv[1]= argv[1];
	childargv[2]= NULL;
	execv("/bin/ls",childargv);
	exit(0);
	}
else error(XTDIO_ERROR,"fork error: <%s>\n",strerror(errno));

return 0;
}

/* ===================================================================== */
/* Modelines: {{{1
 *  vim: fdm=marker fenc=utf-8 ts=4
 */

