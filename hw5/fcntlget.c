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
fd= open(fname,O_CREAT,S_IRUSR);
Dprintf((1,"file<%s> %s\n",sprt(fname),(fd >= 0)? "opened successfully" : "FAILED TO OPEN"));
if(fd < 0) error(XTDIO_ERROR,"file<%s> failed to open: %s\n",fname,strerror(errno));

/* F_GETFL: get file status flags */
Dprintf((1,"attempt to get file status flags\n"));
result= fcntl(fd,F_GETFL);
if(result >= 0) {
	printf("file<%s>: file status flags=%6xx %s%s%s%s%s%s%s%s%s%s%s%s%s%s\n",
	  fname,
	  result,
	  (result & O_LARGEFILE)?     " O_LARGEFILE" : "",
	  (result & O_CREAT)?         " O_CREAT"     : "",
	  (result & O_APPEND)?        " O_APPEND"    : "",
	  (result & O_ASYNC)?         " O_ASYNC"     : "",
	  (result & O_CLOEXEC)?       " O_CLOEXEC"   : "",
	  (result & O_DIRECT)?        " O_DIRECT"    : "",
	  (result & O_DIRECTORY)?     " O_DIRECTORY" : "",
	  (result & O_EXCL)?          " O_EXCL"      : "",
	  (result & O_NOATIME)?       " O_NOATIME"   : "",
	  (result & O_NOCTTY)?        " O_NOCTTY"    : "",
	  (result & O_NOFOLLOW)?      " O_NOFOLLOW"  : "",
	  (result & O_NONBLOCK)?      " O_NONBLOCK"  : "",
	  (result & O_SYNC)?          " O_SYNC"      : "",
	  (result & O_TRUNC)?         " O_TRUNC"     : "");
	printf("  O_LARGEFILE=%6xx & [result=%xx]=%xx\n", O_LARGEFILE, result, (O_LARGEFILE & result));
	printf("  O_CREAT    =%6xx & [result=%xx]=%xx  (not fcntl)\n", O_CREAT    , result, (O_CREAT & result));
	printf("  O_EXCL     =%6xx & [result=%xx]=%xx  (not fcntl)\n", O_EXCL     , result, (O_EXCL & result));
	printf("  O_NOCTTY   =%6xx & [result=%xx]=%xx  (not fcntl; for controlling terminals)\n", O_NOCTTY   , result, (O_NOCTTY & result));
	printf("  O_TRUNC    =%6xx & [result=%xx]=%xx  (not fcntl)\n", O_TRUNC    , result, (O_TRUNC & result));
	printf("  O_APPEND   =%6xx & [result=%xx]=%xx  (file opened in append mode)\n", O_APPEND   , result, (O_APPEND & result));
	printf("  O_NONBLOCK =%6xx & [result=%xx]=%xx  (file opened in nonblocking mode)\n", O_NONBLOCK , result, (O_NONBLOCK & result));
	printf("  O_ASYNC    =%6xx & [result=%xx]=%xx  (enable signal-driven i/o)\n", O_ASYNC    , result, (O_ASYNC & result));
	printf("  O_DIRECT   =%6xx & [result=%xx]=%xx  (minimize cache effects, i/o is synchronous)\n", O_DIRECT   , result, (O_DIRECT & result));
	printf("  O_DIRECTORY=%6xx & [result=%xx]=%xx  (must be a directory)\n", O_DIRECTORY, result, (O_DIRECTORY & result));
	printf("  O_NOFOLLOW =%6xx & [result=%xx]=%xx  (don't follow links)\n", O_NOFOLLOW , result, (O_NOFOLLOW & result));
	printf("  O_NOATIME  =%6xx & [result=%xx]=%xx  (don't update last access time)\n", O_NOATIME  , result, (O_NOATIME & result));
	printf("  O_CLOEXEC  =%6xx & [result=%xx]=%xx  (set close on exec)\n", O_CLOEXEC  , result, (O_CLOEXEC & result));
	printf("  O_SYNC     =%6xx & [result=%xx]=%xx  (only implemented for block devices and files)\n", O_SYNC     , result, (O_SYNC & result));
	printf("  file<%s>: low order two bits meaning: %s%s%s\n",
	  fname,
	  ((result & O_ACCMODE) == O_RDONLY)? " O_RDONLY"    : "",
	  ((result & O_ACCMODE) == O_WRONLY)? " O_WRONLY"    : "",
	  ((result & O_ACCMODE) == O_RDWR)?   " O_RDWR"      : "");
	}
else {
	perror("F_GETFL failed");
	exit(-1);
	}

/* get close-on-exec flag */
Dprintf((1,"attempt to get close-on-exec flag\n"));
result= fcntl(fd,F_GETFD);
if(result >= 0) printf("file<%s>: close-on-exec=%d\n",fname,result);
else            printf("F_GETFD failed");

/* get advisory locks */
Dprintf((1,"attempt to get advisory locks\n"));
flock.l_type   = F_RDLCK;
flock.l_whence = SEEK_CUR;
flock.l_start  = 0;
flock.l_len    = 1;
flock.l_pid    = 0;
result         = fcntl(fd,F_GETLK,&flock);
if(result >= 0) {
	printf("file<%s>: file lock type<%s> whence<%s> start=%ld len=%ld pid=%u\n",
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
	}
else perror("F_GETLK failed");

Rdbg(("main 0"));
return 0;
}

/* ===================================================================== */
/* Modelines: {{{1
 *  vim: fdm=marker fenc=utf-8 ts=4
 */
