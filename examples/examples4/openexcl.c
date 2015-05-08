/* openexcl.c: this program illustrates open with O_EXCL
 *             It attempts to "exclusively" open a file <openexcl.1>
 *             with -rw------- permission.
 *   Author: Charles E. Campbell
 *   Date  : Jan 27, 2012
 */

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
#include <errno.h>
#include "xtdio.h"

/* ========================================================================
 * Functions: {{{1
 */

/* --------------------------------------------------------------------- */
/* main: {{{2 */
int main(
  int    argc,
  char **argv)
{
int fd;

Initdbg((&argc,argv));
rdcolor(); /* sets up YELLOW and NRML ansi escape sequences */

errno = 0;
fd    = open("openexcl.1",O_CREAT|O_EXCL,S_IRUSR|S_IWUSR);
Dprintf((1,"fd=%d  errno=%d<%s>\n",fd,errno,strerror(errno)));
if(fd > 0) {
	Dprintf((1,"closing file descriptor#%d\n",fd));
	close(fd);
	}
else fprintf(stderr,"%s***%swarning%s*** unable to open file<%sopenexcl.1%s> exclusively\n",NRML,YELLOW,NRML,YELLOW,NRML);

Rdbg(("main 0"));
return 0;
}

/* ===================================================================== */
/* Modelines: {{{1
 *  vim: fdm=marker fenc=utf-8 ts=4
 */
