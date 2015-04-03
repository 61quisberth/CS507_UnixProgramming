/* waiter.c: this program illustrates wait() calls
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
#include <wait.h>
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
int   ret= 0;

if(argc > 1) sscanf(argv[1],"%d",&ret);

if((pid= fork()) > 0) {
	pid_t childpid;
	int   status;
	/* parent process */
	printf("I'm the parent process: pid=%xx  ppid=%xx pgrp=%xx pgid=%xx\n",getpid(),getppid(),getpgrp(),getpgid(getpid()));
	childpid= wait(&status);
	printf("ok, the parent is done waiting: childpid=%xx status=%xx\n",childpid,status);
	printf("   WIFEXITED   = %d\n",WIFEXITED(status));
	printf("   WEXITSTATUS = %d\n",WEXITSTATUS(status));
	printf("   WIFSIGNALED = %d\n",WIFSIGNALED(status));
	printf("   WTERMSIG    = %d\n",WTERMSIG(status));
	printf("   WIFSTOPPED  = %d\n",WIFSTOPPED(status));
	printf("   WSTOPSIG    = %d\n",WSTOPSIG(status));
	_exit(0);
	}
else if(pid == 0) {
	/* child process */
	printf("I'm the child  process: pid=%xx  ppid=%xx pgrp=%xx pgid=%xx\n",getpid(),getppid(),getpgrp(),getpgid(getpid()));
	sleep(3);
	printf("The child is back!\n");
	_exit(ret);
	}
else error(XTDIO_ERROR,"fork error: <%s>\n",strerror(errno));

return 0;
}

/* ===================================================================== */
/* Modelines: {{{1
 *  vim: fdm=marker fenc=utf-8 ts=4
 */
