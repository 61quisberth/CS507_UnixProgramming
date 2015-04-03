/* sigsetjmp.c: this program illustrates the usage of sigsetjmp/longsetjmp
 *   Author: Charles E. Campbell, Jr.
 *   Date:   Oct 22, 2010
 *
 *   Exercise this program by using   kill -USR1 (pid)
 *                                    kill -USR2 (pid)
 */

/* =====================================================================
 * Header Section: {{{1
 */

/* ---------------------------------------------------------------------
 * Includes: {{{2
 */
#include <setjmp.h>
#include <signal.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include "xtdio.h"

/* ---------------------------------------------------------------------
 * Global Variables: {{{2
 */
static sigjmp_buf  jmpbuf;
static char       *mesg   = NULL;

/* ---------------------------------------------------------------------
 * Prototypes: {{{2
 */
int main( int, char **);   /* sigsetjmp.c */
void handler_sigusr1(int); /* sigsetjmp.c */
void handler_sigusr2(int); /* sigsetjmp.c */

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

if(signal(SIGUSR1,handler_sigusr1) == SIG_ERR) {
	perror("(sigset sigusr1)");
	exit(0);
	}
if(signal(SIGUSR2,handler_sigusr2) == SIG_ERR) {
	perror("(sigset sigusr2)");
	exit(0);
	}

/* get message to user, including pid of program */
printf("setsetjmp/longsetjmp example  (pid#%d)\n",getpid());
printf("..handles sigusr1\n");
printf("..handles sigusr2 and does a siglongjmp()\n");
printf("..use kill -s [SIGUSR1|SIGUSR2] %d to send signals\n",getpid());

if(sigsetjmp(jmpbuf,1)) {
	/* siglongjmp() causes sigsetjmp() to return 2 here */
	printf("mesg<%s> (after sigsetjmp)\n",mesg);
	exit(0);
	}
/* first time through, sigsetjmp returns 0 */

while(1) {
	pause();
	printf("mesg<%s>\n",mesg);
	}

Rdbg(("main 0"));
return 0;
}

/* --------------------------------------------------------------------- */
/* handler_sigusr1: this function handles sigusr1 signals {{{2 */
void handler_sigusr1(int signum)
{
mesg= "sigusr1";
}

/* --------------------------------------------------------------------- */
/* handler_sigusr2: this function handles sigusr2 signals {{{2 */
void handler_sigusr2(int signum)
{
mesg= "sigusr2 -- using sigsetjmp";
siglongjmp(jmpbuf,2);
}

/* ===================================================================== */
/* Modelines: {{{1
 *  vim: fdm=marker fenc=utf-8 ts=4
 */
