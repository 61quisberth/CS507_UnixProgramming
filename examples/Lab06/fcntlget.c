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
	error(XTDIO_USAGE,"exfcntl filename\n");
	exit(0);
	}
fname= argv[1];

/* check for read access.  If successful, open the file for reading */
if(access(fname,R_OK) == -1) error(XTDIO_ERROR,"no read access for file<%s> (%s)\n",fname,strerror(errno));
fd= open(fname,O_CREAT,S_IRUSR);

/* F_GETFL: get file status flags */
result= fcntl(fd,F_GETFL);
if(result >= 0) printf("<%s>: file status flags=0%o=%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s\n",
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
