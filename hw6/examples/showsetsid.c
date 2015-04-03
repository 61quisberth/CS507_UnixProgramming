/* showsetsid.c: this program shows the effect of a setsid() call
 *   Author: Charles E. Campbell
 *   Date:   Feb 19, 2015
 */

/* =====================================================================
 * Header Section: {{{1
 */

/* ---------------------------------------------------------------------
 * Includes: {{{2
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>

/* ---------------------------------------------------------------------
 * Definitions: {{{2
 */
#define outfile	"/home/cec/showsetsid.out"

/* ========================================================================
 * Functions: {{{1
 */

/* --------------------------------------------------------------------- */
/* main: {{{2 */
int main(
  int    argc,
  char **argv)
{
FILE *fp;
pid_t pid;
pid_t sid;
printf("opening %s\n",outfile);
fp= fopen(outfile,"w");
if(!fp) {
	fprintf(stderr,"***error*** unable to open %s for writing\n",outfile);
	exit(-1);
	}
fprintf(fp,"before setsid: pid=%5xx  ppid=%5xx  groupid=%5xx  sessionid=%5xx  uid=%5xx  euid=%5xx\n",
  getpid(),
  getppid(),
  getpgrp(),
  getsid(getpid()),
  getuid(),
  geteuid());
fclose(fp);
close(0);
close(1);
close(2);
if((pid= fork()) < 0) { /* fork error */
	fprintf(fp,"fork failed: %s\n",strerror(errno));
	fclose(fp);
	exit(-1);
	}
else if(pid == 0) { /* fork: child */
	fp= fopen(outfile,"a");
	if((sid= setsid()) == -1) {
		fprintf(fp,"setsid failed: %s\n",strerror(errno));
		fclose(fp);
		exit(-1);
		}
	fprintf(fp,"after  setsid: pid=%5xx  ppid=%5xx  groupid=%5xx  sessionid=%5xx  uid=%5xx  euid=%5xx  sid=%5xx\n",
	  getpid(),
	  getppid(),
	  getpgrp(),
	  getsid(getpid()),
	  getuid(),
	  geteuid(),
	  sid);
	fclose(fp);
	return 0;
	}
else { /* fork: parent */
	int status;
	fp= fopen(outfile,"a");
	fprintf(fp,"parent: waiting for child to terminate\n");
	wait(&status); /* wait for child to terminate */
	fclose(fp);
	return 0;
	}
}

/* ===================================================================== */
/* Modelines: {{{1
 *  vim: fdm=marker fenc=utf-8 ts=4
 */
