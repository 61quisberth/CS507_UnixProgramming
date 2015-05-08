/* hw7c1.c: this program
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
#include <signal.h>
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
int sigcnt = -1;
int signo[32];
int sigpid = 0;

/* ------------------------------------------------------------------------
 * Explanation: {{{2
 */

/* ------------------------------------------------------------------------
 * Prototypes: {{{2
 */
int main( int, char **);                    /* hw7c1.c */
void sighandler( int, siginfo_t *, void *); /* hw7c1.c */

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

Initdbg((&argc,argv));
rdcolor();
printf("hw7c1 pid#%d\n",getpid());

sigemptyset(&mask);
act.sa_sigaction = sighandler;
act.sa_mask      = mask;
act.sa_flags     = SA_SIGINFO;
act.sa_restorer  = NULL;
sigcnt           = -1;
sigpid           = 0;

/* install signal handlers */
Dprintf((1,"hw7c1 installing signal handlers\n"));
for(isig= 1; isig < 32; ++isig) if(isig != SIGKILL && isig != 19) {
	if(sigaction(isig,&act,&oldact) == -1) {
		printf("hw7c1 sigaction(isig#%d,...): %s\n",isig,strerror(errno));
		exit(-1);
		}
	}

/* print out every signal it got along with the pid of the process that sent the signal.
 * Don't terminate on SIGUSR2 unless signal from parent
 */
while(signo[0] != SIGUSR2 || sigpid != getppid()) {
	sigcnt= -1;
	pause();
	for(isig= 0; isig <= sigcnt; ++isig) printf("Rcvd signal %2d<%s> from pid#%d\n",
	  signo[isig],
	  strsignal(signo[isig]),
	  sigpid);
	sigcnt= -1;
	}

Rdbg(("main 0 : hw7c1 terminating"));
return 0;
}

/* --------------------------------------------------------------------- */
/* sighandler: this function is used to intercept *all* signals (that it can) {{{2 */
void sighandler(
  int        signum,  
  siginfo_t *siginfo, 
  void      *ucontext)
{
Edbg(("sighandler(signum=%d,%ssiginfo,%sucontext)",
  signum,
  siginfo?  "" : "null ",
  ucontext? "" : "null "));

signo[++sigcnt] = signum;                       /* record the signal rcvd          */
sigpid          = siginfo? siginfo->si_pid : 0; /* record the sending process' pid */

Rdbg(("sighandler"));
}

/* ===================================================================== */
/* Modelines: {{{1
 *  vim: fdm=marker fenc=utf-8 ts=4
 */
