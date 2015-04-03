/* timer.c: this program illustrates setting up a timer
 * Author: Charles E. Campbell, Jr.
 * Date  : Oct 17, 2008
 */

/* =====================================================================
 * Header Section: {{{1
 */

/* ---------------------------------------------------------------------
 * Includes: {{{2
 */
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
#include "xtdio.h"

/* ---------------------------------------------------------------------
 * Prototypes: {{{2
 */
int main( int, char **);                        /* timer.c */
int TimerAlarm(int, int); /* timer.c */

/* =====================================================================
 * Functions: {{{1
 */

/* --------------------------------------------------------------------- */
/* main: {{{2 */
int main(
  int    argc,
  char **argv)
{
struct itimerval  itval;

Initdbg((&argc,argv));
rdcolor();

/* install SIGALRM signal handler */
signal((int) SIGALRM,(void (*)()) TimerAlarm);
Dprintf((1,"signal handler now installed\n"));

/* set up itimerval structure */
itval.it_interval.tv_sec = itval.it_interval.tv_usec = 0L;
itval.it_value.tv_usec   = 0L;
if(argc > 1) sscanf(argv[1],"%ld",&itval.it_value.tv_sec);
else         itval.it_value.tv_sec    = 5L;
printf("setitimer for %ld seconds\n",itval.it_value.tv_sec);

/* set up timer */
if(setitimer(ITIMER_REAL,&itval,NULL) < 0) {
	perror("setitimer: ");
	Rdbg(("main 0 : setitimer failure"));
	return 0;
	}
pause();
printf("alarm arrived\n");

Rdbg(("main 0"));
return 0;
}

/* --------------------------------------------------------------------- */
/* TimerAlarm: this function is called when the timer's SIGARLM hits {{{2 */
int TimerAlarm(
  int sig,
  int code)
{
/* re-install SIGALRM handler immediately */
signal((int) SIGALRM,(void (*)()) TimerAlarm);

return 0;
}

/* ===================================================================== */
/* Modelines: {{{1
 *  vim: fdm=marker
 */
