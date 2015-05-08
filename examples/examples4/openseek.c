/* openseek.c: this program illustrates opening, writing, and reading a file
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

/* ------------------------------------------------------------------------
 * Definitions: {{{2
 */
#define BUFSIZE	256

/* =====================================================================
 * Functions: {{{1
 */

/* --------------------------------------------------------------------- */
/* main: {{{2 */
int main(
  int    argc,
  char **argv)
{
char         buf[BUFSIZE];
int          fd;
mode_t       omask;
static char *str1         = "This is just a test";
static char *filename     = "openseek.1";

Initdbg((&argc,argv));

omask= umask(0); /* get copy of old mask */
umask(omask);    /* restore mask         */
printf("Old Mask: %03o\n",omask);

/* open the file:
 *   for reading and writing
 *   create it
 *   truncate it (ie. zero out any previous contents)
 *   set read&write permissions for user (nothing for group or other)
 */
fd= open(filename,O_RDWR | O_CREAT | O_TRUNC,S_IRUSR|S_IWUSR);
if(fd < 0) {
	perror("unable to open <openseek.1>: ");
	exit(1);
	}

/* write to file, read some back */
write(fd,str1,strlen(str1));	/* note that this bumps the current file offset    */
printf("wrote <%s> to file<%s>\n",str1,filename);
lseek(fd,(off_t) 0,SEEK_SET);	/* change current file offset to beginning of file */
read(fd,buf,5);					/* just read five bytes from beginning into buf    */
buf[5]= '\0';					/* make it into a legal C string                   */
close(fd);						/* close file                                      */

printf("buf   <%s>  (seeked to beginning, read 5 bytes)\n",buf);

Rdbg(("main 0"));
return 0;
}

/* ===================================================================== */
/* Modelines: {{{1
 *  vim: fdm=marker fenc=utf-8 ts=4
 */
