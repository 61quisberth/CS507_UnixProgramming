/* fcntlget.c: this program gives examples of fcntl use with F_GETFL and F_GETLK
 *   Author: Charles E. Campbell, Jr.
 *   Date:   Oct 01, 2010
 */

/* =====================================================================
 * Header Section: {{{1
 */

/* ---------------------------------------------------------------------
 * Includes: {{{2
 */
#include <stdio.h>
#include <unistd.h>
#define __USE_GNU
#define __USE_LARGEFILE64
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
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
char         *fname;
int           fd;
int           result;
struct flock  flock;

Initdbg((&argc,argv));
rdcolor();

if(argc <= 1) {
	error(XTDIO_USAGE,"fcntlget filename\n");
	exit(0);
	}
fname= argv[1];

/* check for read access.  If successful, open the file for reading */
if(access(fname,R_OK) == -1) error(XTDIO_ERROR,"no read access for file<%s> (%s)\n",fname,strerror(errno));
fd= open(fname,O_CREAT|O_RDWR,S_IRUSR);

/* F_GETFL: get file status flags */
result= fcntl(fd,F_GETFL);
if(result >= 0) {
	printf("<%s>: file status flags=0%o=%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s\n",
	  fname,
	  result,
	  (result & O_APPEND)?    " O_APPEND"    : "",
	  (result & O_ASYNC)?     " O_ASYNC"     : "",
	  (result & O_CLOEXEC)?   " O_CLOEXEC"   : "",
	  (result & O_CREAT)?     " O_CREAT"     : "",
	  (result & O_DIRECT)?    " O_DIRECT"    : "",
	  (result & O_DIRECTORY)? " O_DIRECTORY" : "",
	  (result & O_EXCL)?      " O_EXCL"      : "",
	  (result & O_LARGEFILE)? " O_LARGEFILE" : "",
	  (result & O_NOATIME)?   " O_NOATIME"   : "",
	  (result & O_NOCTTY)?    " O_NOCTTY"    : "",
	  (result & O_NOFOLLOW)?  " O_NOFOLLOW"  : "",
	  (result & O_NONBLOCK)?  " O_NONBLOCK"  : "",
	  (result & O_RDONLY)?    " O_RDONLY"    : "",
	  (result & O_RDWR)?      " O_RDWR"      : "",
	  (result & O_SYNC)?      " O_SYNC"      : "",
	  (result & O_TRUNC)?     " O_TRUNC"     : "",
	  (result & O_WRONLY)?    " O_WRONLY"    : "");
	printf("result               = %sb\n",binprt(24,(unsigned long)result));
	printf("O_RDONLY    =%7d = %sb   %s\n",O_RDONLY   ,binprt(24,(unsigned long)O_RDONLY   ),"read only access");
	printf("O_WRONLY    =%7d = %sb   %s\n",O_WRONLY   ,binprt(24,(unsigned long)O_WRONLY   ),"write only access");
	printf("O_RDWR      =%7d = %sb   %s\n",O_RDWR     ,binprt(24,(unsigned long)O_RDWR     ),"read and write access");
	printf("O_CREAT     =%7d = %sb   %s\n",O_CREAT    ,binprt(24,(unsigned long)O_CREAT    ),"if the file does not exist it will be created");
	printf("O_EXCL      =%7d = %sb   %s\n",O_EXCL     ,binprt(24,(unsigned long)O_EXCL     ),"exclusive open (open will fail if file already exists)");
	printf("O_NOCTTY    =%7d = %sb   %s\n",O_NOCTTY   ,binprt(24,(unsigned long)O_NOCTTY   ),"opening will not cause the tty to become the controlling terminal for the process");
	printf("O_TRUNC     =%7d = %sb   %s\n",O_TRUNC    ,binprt(24,(unsigned long)O_TRUNC    ),"truncate");
	printf("O_APPEND    =%7d = %sb   %s\n",O_APPEND   ,binprt(24,(unsigned long)O_APPEND   ),"open for append");
	printf("O_NONBLOCK  =%7d = %sb   %s\n",O_NONBLOCK ,binprt(24,(unsigned long)O_NONBLOCK ),"non-blocking open");
	printf("O_SYNC      =%7d = %sb   %s\n",O_SYNC     ,binprt(24,(unsigned long)O_SYNC     ),"requires that any write operations block until all data and all metadata have been written to persistent storage");
	printf("O_ASYNC     =%7d = %sb   %s\n",O_ASYNC    ,binprt(24,(unsigned long)O_ASYNC    ),"generate a signal (SIGIO) when i/o possible");
	printf("O_DIRECT    =%7d = %sb   %s\n",O_DIRECT   ,binprt(24,(unsigned long)O_DIRECT   ),"file i/o is done directly to/from user space buffers");
	printf("O_DIRECTORY =%7d = %sb   %s\n",O_DIRECTORY,binprt(24,(unsigned long)O_DIRECTORY),"if pathname is not a directory, cause the open to fail");
	printf("O_NOFOLLOW  =%7d = %sb   %s\n",O_NOFOLLOW ,binprt(24,(unsigned long)O_NOFOLLOW ),"if pathname is a symbolic link, then the open fails");
	printf("O_NOATIME   =%7d = %sb   %s\n",O_NOATIME  ,binprt(24,(unsigned long)O_NOATIME  ),"do not update the file last access time");
	printf("O_CLOEXEC   =%7d = %sb   %s\n",O_CLOEXEC  ,binprt(24,(unsigned long)O_CLOEXEC  ),"enable the close-on-exec flag");
	printf("O_LARGEFILE =%7d = %sb   %s\n",O_LARGEFILE,binprt(24,(unsigned long)O_LARGEFILE),"allow files whose sizes cannot be represented in an off_t (but is representable using off64_t) to be opened");
	}
else perror("F_GETFL failed");

/* get close-on-exec flag */
result= fcntl(fd,F_GETFD);
if(result >= 0) printf("<%s>: close-on-exec=%d\n",fname,result);
else            printf("F_GETFD failed");

/* get advisory locks */
flock.l_type   = F_RDLCK;
flock.l_whence = SEEK_CUR;
flock.l_start  = 0;
flock.l_len    = 1;
flock.l_pid    = 0;
result         = fcntl(fd,F_GETLK,&flock);
if(result >= 0) printf("<%s>: file lock type<%s> whence<%s> start=%ld len=%ld pid=%u\n",
  fname,
  (flock.l_type   == F_RDLCK)?  "rdlck"    :
  (flock.l_type   == F_WRLCK)?  "wrlck"    :
  (flock.l_type   == F_UNLCK)?  "unlck"    : "???",
  (flock.l_whence == SEEK_SET)? "seek_set" :
  (flock.l_whence == SEEK_CUR)? "seek_cur" :
  (flock.l_whence == SEEK_END)? "seek_end" : "???",
  flock.l_start,
  flock.l_len,
  flock.l_pid);
else perror("F_GETLK failed");

Rdbg(("main 0"));
return 0;
}

/* ===================================================================== */
/* Modelines: {{{1
 *  vim: fdm=marker fenc=utf-8 ts=4
 */
