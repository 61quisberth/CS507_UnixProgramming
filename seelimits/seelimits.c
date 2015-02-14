/* seelimits.c: */

/* =====================================================================
 * Header Section: {{{1
 */

/* ---------------------------------------------------------------------
 * Includes: {{{2
 */
#include	<errno.h>		/* for definition of errno */
#include	<stdarg.h>		/* ANSI C header file */
#include	"xtdio.h"
#include	"seelimits.h"

/* ---------------------------------------------------------------------
 * Globals: {{{2
 */
int   dflag = 0;    /* indicates if we want details or not */
char *pname = NULL; /* caller can set this from argv[0]    */

/* ---------------------------------------------------------------------
 * Prototypes: {{{2
 */
int main(int, char *[]);                                   /* ex2.c           */
static void pr_sysconf(char *, int, char *);               /* ex2.c           */
static void pr_pathconf(char *, char *, int, char *);      /* ex2.c           */
void err_ret(const char *, ...);                           /* ex2.c           */
void err_sys(const char *, ...);                           /* ex2.c           */
void err_dump(const char *, ...);                          /* ex2.c           */
void err_msg(const char *, ...);                           /* ex2.c           */
void err_quit(const char *, ...);                          /* ex2.c           */
static void err_doit(int, const char *, va_list);          /* ex2.c           */

/* =====================================================================
 * Functions: {{{1
 */

/* --------------------------------------------------------------------- */
/* main: {{{2 */
int main(int argc, char *argv[])
{
char *dname=NULL;

Initdbg((&argc,argv));

/* quick&dirty -d option handling */
if(argc >= 2 && !strcmp(argv[1],"-d")) {
	--argc, ++argv;
	dflag= 1;
	}

/* allow current directory to be default */
dname= (argc > 1)? argv[1] : ".";

/* system constants (#defines, mostly) */
printf("\npr_sysconf:\n");
pr_sysconf("_SC_ARG_MAX          = " , _SC_ARG_MAX        , "maxlen of args (in bytes)");
pr_sysconf("_SC_CHILD_MAX        = " , _SC_CHILD_MAX      , "max qty processes/real id");
pr_sysconf("_SC_CLK_TCK          = " , _SC_CLK_TCK        , "clock ticks/second");
#ifdef _SC_HOST_NAME_MAX
pr_sysconf("_SC_HOST_NAME_MAX    = " , _SC_HOST_NAME_MAX  , "max length of a hostname");
#endif
#ifdef _SC_LOGIN_NAME_MAX
pr_sysconf("_SC_LOGIN_NAME_MAX   = " , _SC_LOGIN_NAME_MAX , "max length of a login name");
#endif
pr_sysconf("_SC_NGROUPS_MAX      = " , _SC_NGROUPS_MAX    , "qty simul process group ids/process");
pr_sysconf("_SC_OPEN_MAX         = " , _SC_OPEN_MAX       , "max files a process can have open");
#ifdef _SC_PAGESIZE
pr_sysconf("_SC_PAGESIZE         = " , _SC_PAGESIZE       , "size of a page in bytes");
#endif
pr_sysconf("_SC_PASS_MAX         = " , _SC_PASS_MAX       , "max qty signif chars in a password");
#ifdef	_SC_STREAM_MAX
pr_sysconf("_SC_STREAM_MAX       = " , _SC_STREAM_MAX     , "max qty std i/o streams/process");
#endif
#ifdef	_SC_TZNAME_MAX
pr_sysconf("_SC_TZNAME_MAX       = " , _SC_TZNAME_MAX     , "max qty bytes for a timezone name");
#endif
pr_sysconf("_SC_JOB_CONTROL      = " , _SC_JOB_CONTROL    , "is job-control supported?");
pr_sysconf("_SC_SAVED_IDS        = " , _SC_SAVED_IDS      , "supports saved set-user/group-id?");
pr_sysconf("_SC_VERSION          = " , _SC_VERSION        , "unix version number");

/* non-standard defines */
#ifdef _SC_PHYS_PAGES
pr_sysconf("_SC_PHYS_PAGES       = " , _SC_PHYS_PAGES     , "qty pages of physical memory");
#endif
#ifdef _SC_AVPHYS_PAGES
pr_sysconf("_SC_AVPHYS_PAGES     = " , _SC_AVPHYS_PAGES     , "qty currently available pages of physical memory");
#endif

/* terminal values */
printf("\npr_pathconf: /dev/tty\n");
pr_pathconf("_PC_MAX_CANON        = " , "/dev/tty"      , _PC_MAX_CANON       , "max qty bytes on terminal canonical input queue");
pr_pathconf("_PC_MAX_INPUT        = " , "/dev/tty"      , _PC_MAX_INPUT       , "max qty bytes space avail on terminal input queue");
pr_pathconf("_PC_VDISABLE         = " , "/dev/tty"      , _PC_VDISABLE        , "certain terminal special chars can be disabled?");

/* directory values */
printf("\npr_pathconf: <%s>\n"        , dname);
pr_pathconf("_PC_LINK_MAX         = " , dname         , _PC_LINK_MAX        , "max value of file's link count");
pr_pathconf("_PC_NAME_MAX         = " , dname         , _PC_NAME_MAX        , "max qty bytes in a filename");
pr_pathconf("_PC_PATH_MAX         = " , dname         , _PC_PATH_MAX        , "max qty bytes in a rltv pathname");
pr_pathconf("_PC_PIPE_BUF         = " , dname         , _PC_PIPE_BUF        , "max qty bytes written atomically to a pipe");
pr_pathconf("_PC_CHOWN_RESTRICTED = " , dname         , _PC_CHOWN_RESTRICTED, "is chown use restricted?");
pr_pathconf("_PC_NO_TRUNC         = " , dname         , _PC_NO_TRUNC        , "do long pathnames generate an error?");

Rdbg(("main 0"));
exit(0);
}

/* --------------------------------------------------------------------- */
/* pr_sysconf: {{{2 */
static void pr_sysconf(char *mesg, int name,char *details)
{
long	val;

Edbg(("pr_sysconf(mesg<%s>,name=%d,details<%s>)",
  mesg,
  name,
  details));

fputs(mesg, stdout);
errno = 0;
val= sysconf(name);

if(val < 0) {
	if(errno != 0) err_sys("sysconf error");
	fputs(" (not defined)\n", stdout);
	}
else printf(" %8ld  %s\n", val,dflag? details : "");

Rdbg(("pr_sysconf"));
}

/* --------------------------------------------------------------------- */
/* pr_pathconf: {{{2 */
static void pr_pathconf(char *mesg, char *path, int name,char *details)
{
long	val;

Edbg(("pr_pathconf(mesg<%s>,path<%s>,name=%d,details<%s>)",
  mesg,
  path,
  name,
  details));

fputs(mesg, stdout);
errno = 0;
val   = pathconf(path, name);
if (val < 0) {
	if (errno != 0) err_sys("pathconf error, path = %s", path);
	fputs(" (no limit)\n", stdout);
	}
else printf(" %8ld  %s\n", val,dflag? details : "");

Rdbg(("pr_pathconf"));
}

/* --------------------------------------------------------------------- */
/* err_ret: {{{2
 * Nonfatal error related to a system call.
 * Print a message and return
 */
void err_ret(const char *fmt, ...)
{
va_list		ap;

Edbg(("err_ret(fmt<%s>,...)",fmt));

va_start(ap, fmt);
err_doit(1, fmt, ap);
va_end(ap);

Rdbg(("err_ret"));
return;
}

/* --------------------------------------------------------------------- */

/* Fatal error related to a system call.
 * Print a message and terminate
 */
void err_sys(const char *fmt, ...)
{
va_list		ap;

Edbg(("err_sys(fmt<%s>,...)",fmt));

va_start(ap, fmt);
err_doit(1, fmt, ap);
va_end(ap);

Rdbg(("err_sys"));
exit(1);
}

/* --------------------------------------------------------------------- */

/* Fatal error related to a system call.
 * Print a message, dump core, and terminate
 */
void err_dump(const char *fmt, ...)
{
va_list		ap;

Edbg(("err_dump(fmt<%s>,...)",fmt));

va_start(ap, fmt);
err_doit(1, fmt, ap);
va_end(ap);
abort();		/* dump core and terminate */
exit(1);		/* shouldn't get here */
}

/* --------------------------------------------------------------------- */

/* Nonfatal error unrelated to a system call.
 * Print a message and return
 */
void err_msg(const char *fmt, ...)
{
va_list		ap;

va_start(ap, fmt);
err_doit(0, fmt, ap);
va_end(ap);
return;
}

/* --------------------------------------------------------------------- */

/* Fatal error unrelated to a system call.
 * Print a message and terminate
 */
void err_quit(const char *fmt, ...)
{
va_list		ap;

Edbg(("err_quit(fmt<%s>,...)",fmt));

va_start(ap, fmt);
err_doit(0, fmt, ap);
va_end(ap);
exit(1);

Rdbg(("err_quit"));
}

/* --------------------------------------------------------------------- */

/* Print a message and return to caller.
 * Caller specifies "errnoflag"
 */
static void err_doit(int errnoflag, const char *fmt, va_list ap)
{
int  errno_save;
char buf[MAXLINE];

Edbg(("err_doit(errnoflag=%d,fmt<%s>,...)",errnoflag,fmt));

errno_save = errno;		/* value caller might want printed */
vsprintf(buf, fmt, ap);
if (errnoflag) sprintf(buf+strlen(buf), ": %s", strerror(errno_save));
strcat(buf, "\n");
fflush(stdout);		/* in case stdout and stderr are the same */
fputs(buf, stderr);
fflush(stderr);		/* SunOS 4.1.* doesn't grok NULL argument */

Rdbg(("err_doit"));
return;
}

/* --------------------------------------------------------------------- */
