/* opencreat.c: this program illustrates O_CREAT (no O_EXCL)
 *              It opens a file <opencreat.1> as -rw-------
 *              Then closes the resulting file descriptor.
 *   Author: Charles E. Campbell
 *   Date  :   Jan 27, 2012
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

errno = 0;
fd    = open("opencreat.1",O_CREAT,S_IRUSR|S_IWUSR);
Dprintf((1,"fd=%d  errno=%d<%s>\n",fd,errno,strerror(errno)));
close(fd);

Rdbg(("main 0"));
return 0;
}

/* ===================================================================== */
/* Modelines: {{{1
 *  vim: fdm=marker fenc=utf-8 ts=4
 */
