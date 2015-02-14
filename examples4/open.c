/* open.c: this program illustrates the open - write/read - close calls
 * Author: Charles Campbell
 * Date  : Sep 24, 2010
 */

/* =====================================================================
 * Header Section: {{{1
 */

/* ---------------------------------------------------------------------
 * Includes: {{{2
 */
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include "xtdio.h"

/* ------------------------------------------------------------------------
 * Definitions: {{{2
 */
#define BUFSIZE	20

/* =====================================================================
 * Functions: {{{1
 */

/* --------------------------------------------------------------------- */
/* main: {{{2 */
int main(
  int    argc,
  char **argv)
{
char    buf[BUFSIZE];
char   *a            = NULL;
int     flags        = 0;    /* fd = open(path,flags,mode): flags */
int     fd           = -1;   /* file descriptor                   */
int     qty          = 0;
mode_t  mode         = 0;    /* read/write/execute mode           */

Initdbg((&argc,argv)); /* start up inline-debugger                             */
rdcolor();             /* reads <rdcolor.dat> definitions for RED, YELLOW, etc */

if(argc <= 2) {
	fprintf(stderr,"Usage: open [r|w|a|c|e|t|b|R{ugo}|W{ugo}|X{ugo}] filename [arguments for writing]\n");
	fprintf(stderr," r      open for read\n");
	fprintf(stderr," w      open for write\n");
	fprintf(stderr," a      open for append\n");
	fprintf(stderr," c      open for create\n");
	fprintf(stderr," e      open for exclusive\n");
	fprintf(stderr," t      open with truncation (discard previous file contents)\n");
	fprintf(stderr," b      open with non-blocking enabled\n");
	fprintf(stderr," R{ugo} open with user/group/owner read  permission\n");
	fprintf(stderr," W{ugo} open with user/group/owner write permission\n");
	fprintf(stderr," X{ugo} open with user/group/owner exec  permission\n");
	exit(0);
	}

/* construct flags and mode: this is done by bitwise-or'ing of the selected flags */
Dprintf((1,"%sconstructing flags&mode from <%s%s%s>\n",NRML,YELLOW,sprt(argv[1]),NRML));
flags = 0;
mode  = 0;

for(a= argv[1]; *a; ++a) {
	Dprintf((2,"a<%s>\n",cprt(*a)));
	Dprintf((3,"[flags=%x] & 0x7=%x  O_RDONLY=%xx  O_WRONLY=%xx  O_RDWR=%xx\n",flags,(flags & 0x7),O_RDONLY,O_WRONLY,O_RDWR));
	switch(*a) {

	case 'r': /* open for read                                         */
		flags&= ~0x3;   /* remove read/write status from flags */
		if(strchr(argv[1],'w')) flags|= O_RDWR;
		else                    flags|= O_RDONLY;
		break;

	case 'w': /* open for write                                        */
		flags&= ~0x3;   /* remove read/write status from flags */
		if(strchr(argv[1],'r')) flags|= O_RDWR;
		else                    flags|= O_WRONLY;
		break;

	case 'a': /* open for append                                       */
		flags|= O_APPEND;
		break;

	case 'c': /* open for create                                       */
		flags|= O_CREAT;
		break;

	case 'e': /* open for exclusive                                    */
		flags|= O_EXCL;
		break;

	case 't': /* open with truncation (discard previous file contents) */
		flags|= O_TRUNC;
		break;

	case 'b': /* open with non-blocking enabled                        */
		flags|= O_NONBLOCK;
		break;

	case 'R': /* open with user/group/owner read  permission           */
		++a;
		switch(*a) {
			case 'u': mode|=  S_IRUSR;     break;
			case 'g': mode|= (S_IRUSR<<3); break;
			case 'o': mode|= (S_IRUSR<<6); break;
			default : error(XTDIO_WARNING,"bad R modifier<%s>\n",cprt(*a)); break;
			}
		break;

	case 'W': /* open with user/group/owner write permission           */
		++a;
		switch(*a) {
			case 'u': mode|=  S_IWUSR;     break;
			case 'g': mode|= (S_IWUSR<<3); break;
			case 'o': mode|= (S_IWUSR<<6); break;
			default : error(XTDIO_WARNING,"bad R modifier<%s>\n",cprt(*a)); break;
			}
		break;

	case 'X': /* open with user/group/owner exec  permission           */
		++a;
		switch(*a) {
			case 'u': mode|=  S_IXUSR;     break;
			case 'g': mode|= (S_IXUSR<<3); break;
			case 'o': mode|= (S_IXUSR<<6); break;
			default : error(XTDIO_WARNING,"bad R modifier<%s>\n",cprt(*a)); break;
			}
		break;

	default:
		error(XTDIO_WARNING,"mode<%s> not supported\n",cprt(*a));
		break;
		} /* mode selection          */
	Dprintf((2,"flags=%xx  mode=%o (octal)\n",flags,mode));
	}     /* for loop handling modes */

/* open file with given mode */
Dprintf((1,"attempting to open file<%s%s%s>\n",YELLOW, sprt(argv[2]), GREEN));
Dprintf((1,"flags=%xx=%s%s%s%s%s  mode=%o=%s%s%s%s%s%s%s%s%s\n",
  flags,
  ((flags&0x3) == O_RDONLY)? "rdonly " : "",
  ((flags&0x3) == O_WRONLY)? "wronly " : "",
  ((flags&0x3) == O_RDWR)?   "rdwr "   : "",
  (flags       &  O_CREAT)?  "creat "  : "",
  (flags       &  O_APPEND)? "append " : "",
  mode,
  (mode        &  S_IRUSR)?  "r"       : "-",
  (mode        &  S_IWUSR)?  "w"       : "-",
  (mode        &  S_IXUSR)?  "x"       : "-",
  (mode>>3     &  S_IRUSR)?  "r"       : "-",
  (mode>>3     &  S_IWUSR)?  "w"       : "-",
  (mode>>3     &  S_IXUSR)?  "x"       : "-",
  (mode>>6     &  S_IRUSR)?  "r"       : "-",
  (mode>>6     &  S_IWUSR)?  "w"       : "-",
  (mode>>6     &  S_IXUSR)?  "x"       : "-"));
errno = 0;
fd    = open(argv[2],flags,mode);
if(fd == -1) {
	fprintf(stderr,"***error*** while attempting to use open(file<%s>,flags=%xx,mode=%o): %s\n",sprt(argv[2]),flags,mode,strerror(errno));
	exit(1);
	}
Dprintf((1,"opened <%s> successfully: fd#%d\n",sprt(argv[2]),fd));

/* attempt to read something */
if((flags&0x3) == O_RDONLY) {
	Dprintf((1,"attempting to read something\n"));
	qty = read(fd,buf,(unsigned) BUFSIZE-2);
	if(qty == -1) {
		perror("attempted to read");
		exit(1);
		}
	buf[BUFSIZE-1] = '\0';						/* insure that buf is null-byte terminated */
	printf("read %d bytes: <%s%s%s>\n",qty,YELLOW,sprt(buf),GREEN);
	}

/* attempt to write something */
if( ((flags & 0x3) == O_WRONLY) || ((flags & O_CREAT) && (mode & S_IWUSR)) ) {
	if(argc >= 4) {	/* write additional args to file */
		Dprintf((1,"attempting to write cmdline args to fd#%d\n",fd));
		for(argv+= 3, argc-= 3; argc; ++argv, --argc) {
			qty= write(fd,*argv,(unsigned) strlen(*argv));
			if(qty == -1) {
				perror("attempted to write");
				exit(1);
				}
			}
		}
	else {
		Dprintf((1,"attempting to write something to fd#%d\n",fd));
		sprintf(buf,"csc507 is great!\n");
		qty= write(fd,buf,strlen(buf));
		if(qty == -1) {
			perror("attempted to write");
			exit(1);
			}
		}
	}

if(fd) {
	Dprintf((1,"closing file descriptor#%d\n",fd));
	close(fd);
	fd= -1;
	}

Rdbg(("main 0"));
return 0;
}

/* ===================================================================== */
/* Modelines: {{{1
 *  vim: fdm=marker fenc=utf-8 ts=4
 */
