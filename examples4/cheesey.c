/* cheesey.c: cheesy, that is to say holey, files!
 *   Author: Charles E. Campbell
 *   Date  : Feb 06, 2013
 */

/* =====================================================================
 * Header Section: {{{1
 */

/* ---------------------------------------------------------------------
 * Includes: {{{2
 */
#include <stdio.h>
#include "xtdio.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

/* =====================================================================
 * Functions: {{{1
 */

/* --------------------------------------------------------------------- */
/* main: {{{2 */
int main(
  int    argc,
  char **argv)
{
int         fd;
static char begin[]  = "Genesis";
static char ending[] = "Revelations";

Initdbg((&argc,argv));

/* open a file:
 *   for reading and writing
 *   create it
 *   truncate any previous contents
 *   user has permission to read and write
 */
printf("opening <cheesey.hole> for read/write\n");
fd= open("cheesey.hole",O_RDWR | O_CREAT | O_TRUNC,S_IRUSR|S_IWUSR);

printf("wrote <%s> at beginning of file\n",begin);
write(fd,begin,strlen(begin));   /* write "Genesis"                                 */
write(fd,"\n",(size_t) 1);       /* append a newline                                */

printf("seeked to tenth byte _after_ the end of file\n");
lseek(fd,(off_t) 10,SEEK_END);   /* seek to the 10th byte after current end-of-file */

printf("at that location, writing <%s>\n",ending);
write(fd,ending,strlen(ending)); /* at the new location, put "Revelations"          */
write(fd,"\n",(size_t) 1);       /* append a newline                                */

close(fd);                       /* close the file                                  */

Rdbg(("main 0"));
return 0;
}

/* ===================================================================== */
/* Modelines: {{{1
 *  vim: fdm=marker fenc=utf-8 ts=4
 */
