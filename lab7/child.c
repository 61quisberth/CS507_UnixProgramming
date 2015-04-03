/* child.c: demonstrating fork, wait, status return, etc.
 *   Code from W. Richard Stevens
 *
 *   The parent process creates three children.
 *   The parent process waits on each child, prints its status.
 *
 *   child#1: simply terminates
 *   child#2: terminates with an abort()
 *   child#3: terminates with a floating point error (divide by zero); ie. a SIGFPE
 */

/* =====================================================================
 * Header Section: {{{1
 */

/* ---------------------------------------------------------------------
 * Includes: {{{2
 */
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

/* ---------------------------------------------------------------------
 * Definitions: {{{2
 */
#define	MAXLINE	4096			/* max line length */

/* ---------------------------------------------------------------------
 * Prototypes: {{{2
 */
int main(void);                                   /* wait1.c */
void pr_exit(int);                                /* wait1.c */
void err_sys(const char *, ...);                  /* wait1.c */
static void err_doit(int, const char *, va_list); /* wait1.c */

/* =====================================================================
 * Functions: {{{1
 */

/* --------------------------------------------------------------------- */
/* main: {{{2 */
int main(void)
{
pid_t	pid;
int		status;
int     zero;

zero= 1;
if    ((pid = fork()) < 0)  err_sys("fork error");
else if(pid == 0)	        exit(7);               /* child                               */
if (wait(&status) != pid)   err_sys("wait error"); /* wait for child                      */
pr_exit(status);                                   /* and print its status                */

if     ((pid = fork()) < 0) err_sys("fork error");
else if(pid == 0)	        abort();               /* child generates SIGABRT             */
if (wait(&status) != pid)   err_sys("wait error"); /* wait for child                      */
pr_exit(status);                                   /* and print its status                */

--zero;
if     ((pid= fork()) < 0)  err_sys("fork error");
else if(pid == 0)	        status /= zero;        /* child: divide by 0 generates SIGFPE */
if (wait(&status) != pid)   err_sys("wait error"); /* wait for child                      */
pr_exit(status);                                   /* and print its status                */

exit(0);
}

/* --------------------------------------------------------------------- */
/* pr_exit: {{{2 */
void pr_exit(int status)
{
if      (WIFEXITED(status))   printf("normal termination, exit status = %d\n",WEXITSTATUS(status));
else if (WIFSIGNALED(status)) printf("abnormal termination, signal number = %d%s\n",
  WTERMSIG(status),
#ifdef	WCOREDUMP
  WCOREDUMP(status) ? " (core file generated)" : ""
#else
  ""
#endif
  );
else if (WIFSTOPPED(status))  printf("child stopped, signal number = %d\n",
WSTOPSIG(status));
}

/* --------------------------------------------------------------------- */
/* err_sys: Fatal error related to a system call. {{{2
 * Print a message and terminate.
 */
void err_sys(const char *fmt, ...)
{
va_list ap;

va_start(ap, fmt);
err_doit(1, fmt, ap);
va_end(ap);
exit(1);
}

/* --------------------------------------------------------------------- */
/* err_doit: Print a message and return to caller. {{{2
 * Caller specifies "errnoflag"
 */
static void err_doit(int errnoflag, const char *fmt, va_list ap)
{
int		errno_save;
char	buf[MAXLINE];

errno_save = errno;     /* value caller might want printed        */
vsprintf(buf, fmt, ap);
if (errnoflag) sprintf(buf+strlen(buf), ": %s", strerror(errno_save));
strcat(buf, "\n");
fflush(stdout);         /* in case stdout and stderr are the same */
fputs(buf, stderr);
fflush(stderr);         /* SunOS 4.1.* doesn't grok NULL argument */
return;
}

/* ===================================================================== */
/* Modelines: {{{1
 *  vim: fdm=marker fenc=utf-8 ts=4
 */
