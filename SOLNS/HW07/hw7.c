/* hw7.c: this program implements HW#7's main program
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
#include <errno.h>
#include <limits.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "xtdio.h"

/* ------------------------------------------------------------------------
 * Definitions: {{{2
 */
#define BUFSIZE	256

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
char  buf[BUFSIZE];
char  child1exe[PATH_MAX + 1];
char  child2exe[PATH_MAX + 1];
char  curdir[PATH_MAX+1];
char *childargv[4];
int   hw7c1;
int   hw7c2;
int   status;

Initdbg((&argc,argv));
rdcolor();

printf("hw7   pid#%d\n",getpid());

/* unfortunately execvp() only looks along an abbreviated path which doesn't include ".".
 * Consequently, build the full path here
 */
if(!getcwd(curdir,PATH_MAX+1)) {
	perror("getting current directory: ");
	exit(-1);
	}
sprintf(child1exe,"%s/hw7c1",curdir);
sprintf(child2exe,"%s/hw7c2",curdir);
Dprintf((1,"child1exe<%s>\n",sprt(child1exe)));
Dprintf((1,"child2exe<%s>\n",sprt(child2exe)));

/* sanity checks */
if(access(child1exe,X_OK)) {
	perror("hw7c1: ");
	exit(-1);
	}
if(access(child2exe,X_OK)) {
	perror("hw7c2: ");
	exit(-1);
	}

childargv[0]= childargv[1]= childargv[2]= childargv[3]= NULL;

/* launch the hw7c1 client */
hw7c1= fork();
if(hw7c1 == 0) { /* child process (hw7c1) */
	childargv[0] = child1exe;
#ifdef DEBUG
	if(dbgtst(1)) {
		childargv[1] = "-@2";
		childargv[2] = NULL;
		}
	else childargv[1]= NULL;
#else
	childargv[1]= NULL;
#endif
	Dprintf((1,"launching hw7c1\n"));
	if(execv(child1exe,childargv) == -1) {
		perror("on execvp'ing hw7c1: ");
		exit(-1);
		}
	}
else if(hw7c1 < 0) { /* error */
	printf("error launching hw7c1: %s\n",strerror(errno));
	exit(-1);
	}

/* launch the hw7c2 client */
hw7c2= fork();
if(hw7c2 == 0) { /* child process (hw7c2) */
	sprintf(buf,"%d",hw7c1);
	childargv[0] = child2exe;
	childargv[1] = buf;
#ifdef DEBUG
	if(dbgtst(1)) {
		childargv[2] = "-@2";
		childargv[3] = NULL;
		}
	else childargv[2]= NULL;
#else
	childargv[2]= NULL;
#endif
	Dprintf((1,"launching hw7c2\n"));
	if(execv(child2exe,childargv) == -1) {
		perror("on execvp'ing hw7c2: ");
		exit(-1);
		}
	}
else if(hw7c2 < 0) { /* error */
	printf("error launching hw7c2: %s\n",strerror(errno));
	exit(-1);
	}

/* parent */
Dprintf((1,"parent waiting on hw7c2 to quit\n"));
waitpid(hw7c2,&status,0); /* wait for client hw7c2 to terminate */
Dprintf((1,"parent sending SIGUSR2 to hw7c1 (pid#%d)\n",hw7c1));
kill(hw7c1,SIGUSR2);      /* server sends a SIGUSR2 to client#1 */
Dprintf((1,"parent waiting on hw7c1 to quit\n"));
waitpid(hw7c1,&status,0); /* wait for client hw7c1 to terminate */
printf("Server terminating\n");

Rdbg(("main 0"));
return 0;
}

/* ===================================================================== */
/* Modelines: {{{1
 *  vim: fdm=marker fenc=utf-8 ts=4
 */
