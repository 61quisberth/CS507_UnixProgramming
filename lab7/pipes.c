/* pipes.c: this program illustrates a use of pipe() */

/* =====================================================================
 * Header Section: {{{1
 */

/* ---------------------------------------------------------------------
 * Includes: {{{2
 */
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <wait.h>
#include "xtdio.h"

/* ------------------------------------------------------------------------
 * Definitions: {{{2
 */
#define BUFSIZE	256

/* =====================================================================
 * Functions: {{{1
 */

/* --------------------------------------------------------------------- */
/* main: {{{2 */
int main(
  int    argc,
  char **argv)
{
pid_t        pid;
int          ret                = 0;
int          p2cfifo[2];
int          c2pfifo[2];
char         bufparent[BUFSIZE];
char         bufchild[BUFSIZE];
static char *helloparent        = "hello parent";
static char *hellochild         = "hello child";
int          helloparentlen;
int          hellochildlen;

if(argc > 1) sscanf(argv[1],"%d",&ret);

helloparentlen= strlen(helloparent) + 1;
hellochildlen= strlen(hellochild) + 1;

/* set up a child-to-parent fifo array with pipe()
 *  Note: fifo[0] is the reading end of the pipe
 *        fifo[1] is the writing end of the pipe
 */
ret= pipe(c2pfifo);
if(ret) {
	perror("c2pfifo: ");
	exit(1);
	}

/* set up a parent-to-child fifo array with pipe() */
ret= pipe(p2cfifo);
if(ret) {
	perror("p2cfifo: ");
	exit(1);
	}

/* demonstrate with a parent and child process */
if((pid= fork()) > 0) {
	/* parent process */
	pid_t childpid;
	int   status;

	/* block until child sends parent a message
	 * Note: fdopen says that output may not be
	 * directly followed by input without an
	 * intervening fseek
	 */
	printf("parent blocking till message arrives\n");
	read(c2pfifo[0],bufparent,helloparentlen);
	printf("parent rcvd<%s>\n",bufparent);

	/* sleep, send message to child */
	sleep(1);
	printf("parent sending msg<%s>\n",hellochild);
	write(p2cfifo[1],hellochild,hellochildlen);

	/* wait on child to exit */
	childpid = wait(&status);
	printf("parent exit'ing\n");
	exit(0);
	}

else if(pid == 0) {
	/* child process */
	sleep(1);
	printf("I'm the child  process: pid=%xx  ppid=%xx pgrp=%xx pgid=%xx\n",getpid(),getppid(),getpgrp(),getpgid(getpid()));

	/* send message to parent */
	printf("child  sending msg<%s>\n",helloparent);
	write(c2pfifo[1],helloparent,helloparentlen);

	/* block until parent sends child a message */
	printf("child  blocking till message arrives\n");
	read(p2cfifo[0],bufchild,hellochildlen);
	printf("child  rcvd<%s>\n",bufchild);

	/* that's all, folks! */
	printf("child  exit'ing\n");
	exit(0);
	}
else error(XTDIO_ERROR,"fork error: <%s>\n",strerror(errno));

return 0;
}

/* ===================================================================== */
/* Modelines: {{{1
 *  vim: fdm=marker fenc=utf-8 ts=4
 */
