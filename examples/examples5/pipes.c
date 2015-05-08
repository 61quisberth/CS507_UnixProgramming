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
int          fifo[2];
char         bufparent[BUFSIZE];
char         bufchild[BUFSIZE];
static char *msg1               = "hello to parent";
static char *msg2               = "hello to child";
int          msg1len;
int          msg2len;

Initdbg((&argc,argv));

if(argc > 1) sscanf(argv[1],"%d",&ret);

msg1len= strlen(msg1) + 1;
msg2len= strlen(msg2) + 1;
Dprintf((1,"msg1<%s> msg1len=%d\n",sprt(msg1),msg1len));
Dprintf((1,"msg2<%s> msg2len=%d\n",sprt(msg2),msg2len));

/* set up a fifo array with pipe()
 *  pipe() sets up a unidirectional data channel
 *  fifo[0] : read end of the pipe
 *  fifo[1] : write end of the pipe
 *  Either process (parent or child) may choose to
 *  read or write from either file descriptor.
 */
Dprintf((1,"set up a fifo array with pipe()\n"));
ret= pipe(fifo);
if(ret) {
	perror("pipe: ");
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
	read(fifo[0],bufparent,msg1len);
	printf("parent rcvd<%s>\n",bufparent);

	/* sleep, send message to child */
	sleep(1);
	printf("parent sending msg<%s>\n",msg2);
	write(fifo[1],msg2,msg2len);

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
	printf("child  sending msg<%s>\n",msg1);
	write(fifo[1],msg1,msg1len);

	/* block until parent sends child a message */
	sleep(1);
	printf("child  blocking till message arrives\n");
	read(fifo[0],bufchild,msg2len);
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
