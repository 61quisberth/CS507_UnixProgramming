/* filelock.c: facilitates file locking (under Unix)
 *     Taken from: W. Richard Stevens  "Advanced Programming in the Unix Environment"
 *
 *     Note: the is_readlock returns an answer: can this process read lock or not?
 *           Only another process having write-locked the file will prevent this one
 *           from (potentially) gaining a read lock
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
#include <errno.h>
#include "xtdio.h"

/* ---------------------------------------------------------------------
 * Definitions: {{{2
 */
#ifndef _FILELOCKS_DEFINED_
# define read_lock   (fd, offset, whence, len) lock_reg(fd , F_SETLK , F_RDLCK, offset, whence, len)
# define readw_lock  (fd, offset, whence, len) lock_reg(fd , F_SETLKW, F_RDLCK, offset, whence, len)
# define write_lock  (fd, offset, whence, len) lock_reg(fd , F_SETLK , F_WRLCK, offset, whence, len)
# define writew_lock (fd, offset, whence, len) lock_reg(fd , F_SETLKW, F_WRLCK, offset, whence, len)
# define un_lock     (fd, offset, whence, len) lock_reg(fd , F_SETLK , F_UNLCK, offset, whence, len)
# define is_readlock (fd, offset, whence, len) lock_test(fd, F_RDLCK , offset , whence, len)
# define is_writelock(fd, offset, whence, len) lock_test(fd, F_WRLCK , offset , whence, len)
#endif


/* =====================================================================
 * Functions: {{{1
 */

/* --------------------------------------------------------------------- */
/* lock_reg: register a (read/write) lock with Unix {{{2 */
int lock_reg(int fd, int cmd, int type, off_t offset, int whence, off_t len)
{
int          ret;
struct flock lock;

Edbg(("lock_reg(fd=%xx,cmd<%s>,type<%s>,offset=%ld,whence<%s>,len=%ld)",
  fd,
  (cmd == F_SETLK)?  "set lock"        :
  (cmd == F_SETLKW)? "set lock & wait" : "???",
  (type == F_RDLCK)? "read"            :
  (type == F_WRLCK)? "write"           :
  (type == F_UNLCK)? "unlock"          : "???",
    offset,
  (whence == SEEK_SET)? "bgn"          :
  (whence == SEEK_CUR)? "cur"          :
  (whence == SEEK_END)? "end"          : "???",
  len));

/* build lock request */
lock.l_type   = type;   /* F_RDLCK, F_WRLCK, F_UNLCK         */
lock.l_start  = offset; /* byte offset, relative to l_whence */
lock.l_whence = whence; /* SEEK_SET, SEEK_CUR, SEEK_END      */
lock.l_len    = len;    /* #bytes (0 means to EOF)           */

/* make lock request */
ret= fcntl(fd, cmd, &lock);
#ifdef DEBUG
if(dbgtst(1)) {
	Dprintf((1,"fcntl failed with %d: %s\n",
	  ret,
	  strerror(errno)));
	}
#endif

Rdbg(("lock_reg %d",ret));
return ret;
}

/* --------------------------------------------------------------------- */
/* lock_test: test if a file descriptor zone is (read/write) locked {{{2 */
pid_t lock_test(int fd, int type, off_t offset, int whence, off_t len)
{
int          ret;
struct flock lock;

Edbg(("lock_test(fd=%xx,type<%s>,offset=%ld,whence<%s>,len=%ld)",
  fd,
  (type == F_RDLCK)? "read"   :
  (type == F_WRLCK)? "write"  : "???",
  offset,
  (whence == SEEK_SET)? "bgn" :
  (whence == SEEK_CUR)? "cur" :
  (whence == SEEK_END)? "end" : "???",
  len));

lock.l_type   = type;   /* F_RDLCK or F_WRLCK                */
lock.l_start  = offset; /* byte offset, relative to l_whence */
lock.l_whence = whence; /* SEEK_SET, SEEK_CUR, SEEK_END      */
lock.l_len    = len;    /* #bytes (0 means to EOF)           */

if(fcntl(fd, F_GETLK, &lock) < 0) perror("fcntl error");

/* false, region is not locked by another proc */
if(lock.l_type == F_UNLCK) ret= 0;          /* false, return nothing          */
else                       ret= lock.l_pid; /* true, return pid of lock owner */

Rdbg(("lock_test %d",ret));
return ret;
}

/* =====================================================================
 * Debug Test: {{{1
 */
#ifdef DEBUG_TEST
#define BUFSIZE	256

/* --------------------------------------------------------------------- */
/* main: test routine begins here... {{{2 */
int main(int argc,char **argv)
{
char   buf2[BUFSIZE];
char   buf[BUFSIZE];
char   swhence[BUFSIZE]; /* bgn cur end                               */
char  *b;
int    fd;
int    ret;
int    items;
off_t  len;              /* #bytes (0 means to EOF)                   */
off_t  offset;           /* byte offset, relative to l_whence         */
off_t  whence;           /* SEEK_SET, SEEK_CUR, SEEK_END              */

Initdbg((&argc,argv));   /* initialize Dr Chip's debugger             */
rdcolor();               /* initialize color names (GREEN, RED, etc.) */

if(argc <= 1) error(XTDIO_ERROR,"needs a filename\n");
fd= open(argv[1],O_RDWR,O_CREAT);	/* open for read&write, create if necessary */
if(fd < 0) {
	perror("open: ");
	exit(1);
	}

while(1) {
	printf("  %sr   %sread_lock\n"   , YELLOW, GREEN);
	printf("  %sR   %sreadw_lock\n"  , YELLOW, GREEN);
	printf("  %sw   %swrite_lock\n"  , YELLOW, GREEN);
	printf("  %sW   %swritew_lock\n" , YELLOW, GREEN);
	printf("  %su   %sun_lock\n"     , YELLOW, GREEN);
	printf(" %s?r   %sis_readlock\n" , YELLOW, GREEN);
	printf(" %s?w   %sis_writelock\n", YELLOW, GREEN);
	printf("  %sq   %squit\n"        , YELLOW, GREEN);
	printf("%sEnter command (%sq%s|%sr%s|%sR%s|%sw%s|%sW%s|%su%s|%s?r%s|%s?w  bgn%s|%scur%s|%send  offset length%s)%s: ",
	  CYAN,GREEN,
	  CYAN,GREEN,
	  CYAN,GREEN,
	  CYAN,GREEN,
	  CYAN,GREEN,
	  CYAN,GREEN,
	  CYAN,GREEN,
	  CYAN,GREEN,
	  CYAN,GREEN,
	  CYAN,GREEN,
	  CYAN,GREEN);
	if(!fgets(buf,BUFSIZE,stdin)) break;

	/* parse input */
	b= stpblk(buf);	/* skip leading whitespace */
	Dprintf((1,"b<%s>\n",sprt(b)));
	if(*b != 'q') {
		offset   = 0L;
		*swhence = '\0';
		len      = 0L;
		items    = sscanf(b,"%s %s %ld %ld",buf2,swhence,&offset,&len);
		Dprintf((1,"%s: offset=%ld whence<%s> len=%ld  items=%d\n",
		  buf2,
		  offset,
		  swhence,
		  len,
		  items));
		}
	else goto thatsall;

	/* parse swhence */
	if     (!strcmp(swhence,"bgn")) whence= SEEK_SET;
	else if(!strcmp(swhence,"cur")) whence= SEEK_CUR;
	else if(!strcmp(swhence,"end")) whence= SEEK_END;
	else if(!*swhence) {
		error(XTDIO_WARNING,"usage: ?[w|r] [bgn|cur|end]\n");
		continue;
		}
	else {
		error(XTDIO_WARNING,"whence<%s> not supported\n",swhence);
		continue;
		}

	Dprintf((2,"b<%c%c>\n",b[0],b[1]));

	switch(*b) {
	case 'q':
		goto thatsall;

	case 'r':
		if(items < 4) error(XTDIO_WARNING,"%c needs [bgn|cur|end] offset len\n",*b);
		else          ret= read_lock(fd,offset,whence,len);
		break;

	case 'R':
		if(items < 4) error(XTDIO_WARNING,"%c needs [bgn|cur|end] offset len\n",*b);
		else          ret= readw_lock(fd,offset,whence,len);
		break;

	case 'w':
		if(items < 4) error(XTDIO_WARNING,"%c needs [bgn|cur|end] offset len\n",*b);
		else          ret= write_lock(fd,offset,whence,len);
		break;

	case 'W':
		if(items < 4) error(XTDIO_WARNING,"%c needs [bgn|cur|end] offset len\n",*b);
		else          ret= writew_lock(fd,offset,whence,len);
		break;

	case 'u':
		if(items < 4) error(XTDIO_WARNING,"%c needs [bgn|cur|end] offset len\n",*b);
		else          ret= un_lock(fd,offset,whence,len);
		break;

	case '?':
		if(b[1] == 'r') {
			if(items < 4) error(XTDIO_WARNING,"?%c needs [bgn|cur|end] offset len\n",b[1]);
			else {
				ret= is_readlock (fd,offset,whence,len);
				if(ret) printf("locked against reads by process #%xx\n",ret);
				else    printf("not locked\n");
				}
			}
		else if(b[1] == 'w') {
			if(items < 4) error(XTDIO_WARNING,"?%c needs [bgn|cur|end] offset len\n",b[1]);
			else {
				ret= is_writelock(fd,offset,whence,len);
				if(ret) printf("locked against writes by process #%xx\n",ret);
				else    printf("not locked\n");
				}
			}
		else {
			error(XTDIO_WARNING,"?%c not supported\n",b[1]);
			continue;
			}
		break;

	default:
		error(XTDIO_WARNING,"<%c> is unsupported\n",*b);
		continue;
		}
	if(ret) printf("operation failed: %s\n",strerror(errno));
	else    printf("operation succeeded\n");
	}

thatsall:
close(fd);

Rdbg(("main 0"));
return 0;
}
#endif	/* DEBUG_TEST */

/* ===================================================================== */
/* Modelines: {{{1
 *  vim: fdm=marker
 */
