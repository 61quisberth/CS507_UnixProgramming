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

/* ---------------------------------------------------------------------
 * Prototypes: {{{2
 */
int main( int, char **);                        /* timer.c */
#ifdef sgi
int TimerAlarm(int, int, struct sigcontext *); /* timer.c */
#else
int TimerAlarm(int, int); /* timer.c */
#endif

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

/* install SIGALRM signal handler */
signal((int) SIGALRM,(void (*)()) TimerAlarm);
signal((int) SIGVTALRM,(void (*)()) TimerAlarm);
signal((int) SIGPROF,(void (*)()) TimerAlarm);

/* set up itimerval structure */
itval.it_interval.tv_sec = itval.it_interval.tv_usec = 0L; /* initialize next-value timer time to zero                   */
itval.it_value.tv_usec   = 0L;                             /* initialize current time to zero                            */
if(argc > 1) sscanf(argv[1],"%ld",&itval.it_value.tv_sec); /* interpret first command line argument into next-value time */
else         itval.it_value.tv_sec    = 5L;                /* default next-value timer value: 5 seconds                  */
printf("setitimer for %ld seconds\n",itval.it_value.tv_sec);

/* set up timer                            (delivers)
 *   ITIMER_REAL   : real timer            (SIGALRM)
 *   ITIMER_VIRTUAL: process time only     (SIGVTALRM)
 *   ITIMER_PROF   : process + system time (SIGPROF)
 */
if(setitimer(ITIMER_REAL,&itval,NULL) < 0) {
	perror("setitimer: ");
	return 0;
	}
pause();
printf("alarm arrived\n");

return 0;
}

/* --------------------------------------------------------------------- */
/* TimerAlarm: this function is called when the timer's signal hits {{{2 */
int TimerAlarm(
  int sig,
  int code)
{
/* reset SIGALRM, SIGVTALRM, SIGPROF handler immediately */
signal((int) sig,(void (*)()) TimerAlarm);
return 0;
}

/* ===================================================================== */
/* Modelines: {{{1
 *  vim: fdm=marker
 */
