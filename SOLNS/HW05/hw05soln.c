/* hw05.c: this program
 *   Author: Charles E. Campbell
 *   Date:   Dec 17, 2010
 */

/* =====================================================================
 * Header Section: {{{1
 */

/* ---------------------------------------------------------------------
 * Includes: {{{2
 */
#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include "xtdio.h"

/* ------------------------------------------------------------------------
 * Definitions: {{{2
 */
#ifdef is_readlock
# undef is_readlock
#endif
#ifdef is_writelock
# undef is_writelock
#endif
#define BUFSIZE                            512
#define is_readlock(fd , offset, whence, len) lock_test(fd, F_RDLCK, offset, whence, len)
#define is_writelock(fd, offset, whence, len) lock_test(fd, F_WRLCK, offset, whence, len)

/* ========================================================================
 * Functions: {{{1
 */

/* --------------------------------------------------------------------- */
/* main: {{{2 */
int main(
  int    argc,
  char **argv)
{
static char  buf[BUFSIZE];
static char  Lbuf[BUFSIZE];
static char  Ubuf[BUFSIZE];
int          iarg;
int          ibuf;
int          buflen;
int          fd;
int          offset;
FILE        *fp;

Initdbg((&argc,argv));
rdcolor();

for(iarg= 1; iarg < argc; ++iarg) {
	if(!access(argv[iarg],R_OK)) {
		fd= open(argv[iarg],O_RDONLY); /* open a file via a file descriptor */
		if(fd == -1) {
			perror("unable to open file for reading: ");
			exit(-1);
			}
		fp     = fdopen(fd,"r"); /* get a stream */
		offset = 0;
		while(fgets(buf,BUFSIZE,fp)) {
			buflen= strlen(buf);
			if(buflen < 80) buflen= 80;
			printf("         ");
			for(ibuf= 0; ibuf < buflen; ++ibuf) {
				if(buf[ibuf] == '\n') continue;
				if(is_writelock(fd,offset+ibuf,SEEK_SET,1)) {
					Lbuf[ibuf]= buf[ibuf];
					Ubuf[ibuf]= ' ';
					}
				else {
					Lbuf[ibuf]= ' ';
					Ubuf[ibuf]= buf[ibuf];
					}
				}
			for(ibuf= 0; ibuf < buflen; ++ibuf) printf("-");
			printf("\n");
			printf("  locked:%s\n",Lbuf);
			printf("unlocked:%s\n",Ubuf);
			offset= ftell(fp);
			}
		printf("         ");
		for(ibuf= 0; ibuf < buflen; ++ibuf) printf("-");
		printf("\n");
		fclose(fp);
		close(fd);
		}
	else error(XTDIO_WARNING,"unable to open file<%s> for reading\n",sprt(argv[iarg]));
	}


Rdbg(("main 0"));
return 0;
}

/* ===================================================================== */
/* Modelines: {{{1
 *  vim: fdm=marker fenc=utf-8 ts=4
 */
