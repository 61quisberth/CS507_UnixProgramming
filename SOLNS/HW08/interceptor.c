/* interceptor.c: this program intercepts all signals, 1-50 (except for those that it cannot intercept)
 *   Author: Charles E. Campbell
 *   Date:   Apr 15, 2011
 */

/* =====================================================================
 * Header Section: {{{1
 */

/* ---------------------------------------------------------------------
 * Includes: {{{2
 */
#include <stdio.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "xtdio.h"

/* ------------------------------------------------------------------------
 * Definitions: {{{2
 */
#define BUFSIZE 256

/* ------------------------------------------------------------------------
 * Typedefs: {{{2
 */

/* ------------------------------------------------------------------------
 * Local Data Structures: {{{2
 */

/* ------------------------------------------------------------------------
 * Global Data: {{{2
 */
int sigcnt = -1;
int signo[32];

/* ------------------------------------------------------------------------
 * Explanation: {{{2
 */

/* ------------------------------------------------------------------------
 * Prototypes: {{{2
 */
int main( int, char **); /* interceptor.c */
void sighandler(int);    /* interceptor.c */

/* ========================================================================
 * Functions: {{{1
 */

/* --------------------------------------------------------------------- */
/* main: {{{2 */
int main(
  int    argc,
  char **argv)
{
struct sigaction act;
struct sigaction oldact;
sigset_t         mask;
int              isig;
int              ret;
char             buf[BUFSIZE];

Initdbg((&argc,argv));
rdcolor();

printf("interceptor pid#%d\n",getpid());

sigemptyset(&mask);
act.sa_handler   = sighandler;
act.sa_mask      = mask;
act.sa_flags     = 0;
act.sa_restorer  = NULL;
sigcnt           = -1;

/* install signal handlers */
Dprintf((1,"interceptor: installing signal handlers\n"));
for(isig= 1; isig <= 31; ++isig) if(isig != SIGKILL && isig != 19) {
	Dprintf((2,"installing sighandler for sig#%d\n",isig));
	if(sigaction(isig,&act,&oldact) == -1) {
		printf("interceptor sigaction(isig#%d,...): %s\n",isig,strerror(errno));
		exit(-1);
		}
	}

printf("Enter \"quit\" to quit: ");
while(1) {
	ret= scanf("%s",buf);
	Dprintf((1,"ret=%d  sigcnt=%d\n",ret,sigcnt));
	if(ret > 0 && !strcmp(buf,"quit")) break;
	else {
		for(isig= 0; isig <= sigcnt; ++isig) printf("Rcvd signal %2d<%s>\n",
		  signo[isig],
		  strsignal(signo[isig]));
		sigcnt= -1;
		}
	}

Rdbg(("main 0"));
return 0;
}

/* --------------------------------------------------------------------- */
/* sighandler: this function is used to intercept *all* signals (that it can) {{{2 */
void sighandler(int signum)
{
Edbg(("sighandler(signum=%d)",signum));

signo[++sigcnt] = signum; /* record the signal(s) rcvd          */

Rdbg(("sighandler"));
}

/* ===================================================================== */
/* Modelines: {{{1
 *  vim: fdm=marker fenc=utf-8 ts=4
 */
