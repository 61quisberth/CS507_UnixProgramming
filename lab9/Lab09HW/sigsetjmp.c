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
#include <stdio.h>
#include <signal.h>
#include <setjmp.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

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
printf("installing sigusr1 handler\n");
if(signal(SIGUSR1,handler_sigusr1) == SIG_ERR) {
	perror("(sigset sigusr1)");
	exit(0);
	}
printf("installing sigusr2 handler\n");
if(signal(SIGUSR2,handler_sigusr2) == SIG_ERR) {
	perror("(sigset sigusr2)");
	exit(0);
	}
printf("setsetjmp/longsetjmp example  (pid#%d)\n",getpid());

printf("calling sigsetjmp\n");
if(sigsetjmp(jmpbuf,1)) {
	/* siglongjmp causes sigsetjmp to return 2 here */
	printf("mesg<%s> (after sigsetjmp)\n",mesg);
	exit(0);
	}
/* first time through, sigsetjmp returns 0 */

printf("pausing until signal received\n");
printf(" use: kill -s SIGUSR1 %d\n",getpid());
printf(" use: kill -s SIGUSR2 %d\n",getpid());
while(1) {
	pause();
	printf("mesg<%s>\n",mesg);
	}

return 0;
}

/* --------------------------------------------------------------------- */
/* handler_sigusr1: this function {{{2 */
void handler_sigusr1(int signum)
{
mesg= "sigusr1";
}

/* --------------------------------------------------------------------- */
/* handler_sigusr2: this function {{{2 */
void handler_sigusr2(int signum)
{
mesg= "sigusr2 -- using sigsetjmp";
siglongjmp(jmpbuf,2);
}

/* ===================================================================== */
/* Modelines: {{{1
 *  vim: fdm=marker fenc=utf-8 ts=4
 */
