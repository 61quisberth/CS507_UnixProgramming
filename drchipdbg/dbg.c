/* dbg.c: this file contains debugging support functions
 *
 *  Author   : Charles E. Campbell, Jr., Ph.D.  (original 11/6/92)
 *  Copyright: 1992 - 2008
 *  Version  : 4e
 *
 *  History:
 *  v4  Jul 17, 2006 : handles concurrent threads
 *      Aug 11, 2006 : detail<0: only dprintf when dbgcmd's detail==|detail|, otherwise as before
 *      Mar 06, 2007 : added -@@ for "on main 1 > tmp"
 *      Jan 16, 2008 : added :dfl (see DFL in dbg.h) to report file and line number to dprintf
 *      Jun 04, 2008 : changed to :nodfl with DFL reporting to be defaulted to enabled
 *  v3  Mar 20, 2006 : moved dtst1/dtst2 setting to before Edbg() prints
 *  v2  Mar 10, 2006 : improved ondbg and off handling
 *  v1  Nov 06, 1992 : initial
 */

/* =====================================================================
 * Header Section: {{{1
 */

/* ---------------------------------------------------------------------
 * Includes: {{{2
 */
#include <stdio.h>
#include <ctype.h>
#include <setjmp.h>
#include <pthread.h>
#if defined(__WIN32__) || defined(__CYGWIN__)
# include "windows.h"
#endif
# define DEBUG
#define DBG_MAIN
#include "xtdio.h"

/* -----------------------------------------------------------------------
 * Definitions Section: {{{2
 */
# define STDOUT stdout
#define DBGBUF        2048     /* size of debug parsing buffer           */
#define DBG_OFF       0        /* status is debugging is off             */
#define DBG_ONDIS     1        /* status is debugging is on but disabled */
#define DBG_ON        2        /* status is debugging is on and enabled  */
#define DBG_KEEP      3        /* status is left unchanged               */
#define FUNCBUF       (2*32+2) /* max function name size                 */
#define DBGTHREADHASH 100
#define NEST          for(thrd->idbg= 0; thrd->idbg < thrd->dbglvl; ++thrd->idbg) fputc(dbgnestchar,thrd->dbgfp);
#if 0
#define DNEST	for(idbg2= 0; idbg2 < (thrd? thrd->dbglvl : 0)+dbglvl2); ++idbg2) fputc('+',stdout);
#else
#define DNEST	{if(thrd) for(idbg2= 0; idbg2 < thrd->dbglvl;  ++idbg2) fputc(dbgnestchar,stdout); for(idbg2= 0; idbg2 < dbglvl2; ++idbg2) fputc('+',stdout); }
#endif

# define STDFP	stderr
# if defined(__WIN32__) || defined(__CYGWIN__)
#  define MUTEXLOCK	EnterCriticalSection(&dbgmutex);
# else
#  define MUTEXLOCK 	pthread_mutex_lock(&dbgmutex);
# endif
# if defined(__WIN32__) || defined(__CYGWIN__)
#  define MUTEXUNLOCK	LeaveCriticalSection(&dbgmutex);
# else
#  define MUTEXUNLOCK	pthread_mutex_unlock(&dbgmutex);
# endif


/* -----------------------------------------------------------------------
 * Typedefs: {{{2
 */
typedef struct DBG_str       DBG;
typedef struct DBGCMD_str    DBGCMD;
typedef struct DBGJMPBUF_str DBGJMPBUF;
typedef struct DBGOUT_str    DBGOUT;
typedef struct DBGPREQ_str   DBGPREQ;
typedef struct DBGTHREAD_str DBGTHREAD;

/* -----------------------------------------------------------------------
 * Structures: {{{2
 *
 * DBG      : edbg/rdbg put DBGs on a stack.  They record debugging status
 * DBGCMD   : debugging commands (on funcname 1, off funcname, etc) stored
 *            on a list
 * DBGJMPBUF: dsetjmp and dlongjmp support
 * DBGOUT   : holds current debugging-output filename and handle
 *
 */
struct DBG_str {                  /* DBG                            {{{2   */
    char    funcname[FUNCBUF];    /* name of function                      */
    char   *dbgswbuf;             /* dbgswitch edbg buffer                 */
    int     status;               /* off/inactive/active                   */
    int     detail;               /* current detail level of debugging     */
    int     inherit;              /* current inheritance mode              */
	int     dbgdfl;               /* en/dis-able dprintf line numbering    */
    int     dtst1;                /* for use by programmer                 */
    int     dtst2;                /* for use by programmer                 */
    DBGOUT *dbgout;               /* current debugging output              */
    DBG    *nxt,*prv;             /* supports current DBG stack            */
    };

struct DBGCMD_str {               /* DBGCMD                     {{{2       */
    char       funcname[FUNCBUF]; /* fixed size to avoid calloc overhead   */
    int        dtst1;             /* for use by programmer                 */
    int        dtst2;             /* for use by programmer                 */
	int        dbgdfl;            /* en/dis-able dprintf line numbering    */
    int        inherit;           /* controls debugging inheritance        */
    int        npreq;             /* qty of pre-requisites                 */
    int        status;            /* DBG_OFF, DBG_ONDIS, DBG_KEEP,  DBG_ON */
    unsigned   detail;            /* level of debugging command            */
    DBGOUT    *dbgout;            /* debugging output goes here            */
    DBGPREQ   *phd,*ptl;          /* pre-requisite list                    */
    DBGCMD    *nxt,*prv;          /* linked list of DBGCMDs                */
    };

struct DBGJMPBUF_str {            /* DBGJMPBUF                  {{{2       */
    void       *env;              /* when going to this jmpbuf environ,    */
    DBG        *dbg;              /* jump to this DBG stack entry          */
    DBGJMPBUF  *nxt,*prv;
    };

struct DBGOUT_str {               /* DBGOUT                     {{{2       */
    char    *filename;            /* dbg output redirected to this file    */
    FILE    *fp;                  /* file system handle                    */
    DBGOUT  *nxt,*prv;            /* linked list of DBGOUTs                */
    };

struct DBGPREQ_str {              /* DBGPREQ                        {{{2   */
    char     funcname[FUNCBUF];   /* fixed size to avoid calloc overhead   */
    char     select;              /* blank ! or +                          */
    DBGPREQ *nxt,*prv;            /* pre-requisite linked list             */
    };

struct DBGTHREAD_str {            /* DBGTHREAD                    {{{2     */
#if defined(__WIN32__) || defined(__CYGWIN__)
	DWORD      self;              /* windows handle                        */
#else
    pthread_t  self;              /* pthread_self() handle                 */
#endif
	int        id;                /* used to support % in output filenames */
    char       dbgbuf[DBGBUF];    /* debugging parsing buffer              */
    int        dbgnestmode;       /* allows dprintfs w/o newlines          */
	int        dbglvl;			  /* current NESTing level				   */
	int        idbg;			  /* used by NEST						   */
    DBG       *dbghd;             /* current function stack head           */
    DBG       *dbgtl;
    DBGJMPBUF *dbgjmphd;          /* jmpbuf support                        */
    DBGJMPBUF *dbgjmptl;
    FILE      *dbgfp;             /* standard debugging output             */
    DBGTHREAD *nxt;               /* linked list of DBGTHREADs             */
    DBGTHREAD *prv;               /* linked list of DBGTHREADs             */
	};

/* -----------------------------------------------------------------------
 * Local Variables: {{{2
 */
static int               showthrdid = 0;
static DBGTHREAD        *thrd       = NULL;
DBGCMD                  *dbgcmdhd   = NULL;                      /* debugging commands         */
DBGCMD                  *dbgcmdtl   = NULL;
DBGOUT                  *dbgouthd   = NULL;                      /* output file support        */
DBGOUT                  *dbgouttl   = NULL;
#if defined(__WIN32__) || defined(__CYGWIN__)
static CRITICAL_SECTION  dbgmutex;
#else
static pthread_mutex_t   dbgmutex;
#endif

/* -----------------------------------------------------------------------
 * Local Prototypes: {{{2
 */
#ifdef __PROTOTYPE__
static void edbg2(DBGCMD *);                    /* dbg.c */
static char *dstptok(char *,char *,int,char *); /* dbg.c */
#else
static void edbg2();                            /* dbg.c */
static char *dstptok();                         /* dbg.c */
#endif
#if !(defined(__WIN32__) || defined(__CYGWIN__))
int pthread_mutexattr_settype(pthread_mutexattr_t *attr, int type);
#endif

#ifdef __WIN32__
# ifdef printf
#  undef printf
# endif
# ifdef fprintf
#  undef fprintf
# endif
# ifdef fputs
#  undef fputs
# endif
# ifdef vfprintf
#  undef vfprintf
# endif
#endif

/* =======================================================================
 * Functions: {{{1
 */

/* --------------------------------------------------------------------- */
/* dcontext: this function uses a hash table to determine the correct {{{2
 *  DBGTHREAD to use.  If there's no match, a new DBGTHREAD is created.
 */
static DBGTHREAD *dcontext(void)
{
int                     hash;
static int              first                 = 1;
static int              id                    = 0;
static DBGTHREAD       *hashhd[DBGTHREADHASH];
static DBGTHREAD       *hashtl[DBGTHREADHASH];
#ifndef sgi
static pthread_mutexattr_t attr;
#endif
#if defined(__WIN32__) || defined(__CYGWIN__)
DWORD                   self;
extern pthread_t        pthread_self(void);
unsigned                uself;
#elif defined(_PTHREAD_H) || defined(_PTHREAD_H_)
pthread_t               self;
unsigned                uself;
#endif


if(first) {
	/* initialize all hash table linked lists' head/tail pointers to NULL */
	for(hash= 0; hash < DBGTHREADHASH; ++hash) hashhd[hash]= hashtl[hash]= NULL;
	first= 0;
#if defined(__WIN32__) || defined(__CYGWIN__)
	/* initialize the critical section */
	InitializeCriticalSectionAndSpinCount(&dbgmutex,0x80000400);
#elif defined(_PTHREAD_H)
	/* initialize a recursive mutex */
	pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE_NP);
    pthread_mutex_init(&dbgmutex,&attr);
#endif
	}

/* identify the proper thrd */
#if defined(__WIN32__) || defined(__CYGWIN__)
self   = GetCurrentThreadId();
memcpy(&uself,&self,sizeof(uself));
uself ^= (uself>>4) ^ (uself >> 24);
hash   = uself%DBGTHREADHASH;
for(thrd= hashhd[hash]; thrd && thrd->self != self; thrd= thrd->nxt) ;

#elif defined(_PTHREAD_H) || defined(_PTHREAD_H_)
self   = pthread_self();
memcpy(&uself,&self,sizeof(uself));
uself ^= (uself>>4) ^ (uself >> 24);
hash   = uself%DBGTHREADHASH;
for(thrd= hashhd[hash]; thrd && !pthread_equal(thrd->self,self); thrd= thrd->nxt) ;
#endif


if(!thrd) { /* initialize a new DBGTHREAD */
	double_link(DBGTHREAD,hashhd[hash],hashtl[hash],"DBGTHREAD allocation failure");
	thrd              = hashtl[hash];
	thrd->self        = self;
    thrd->id          = ++id;   /* provide a thread id for % in dbg output filenames */
    thrd->dbgbuf[0]   = '\0';
    thrd->dbgnestmode = 0;      /* allows dprintfs w/o newlines                      */
    thrd->dbglvl      = 0;
    thrd->idbg        = 0;
    thrd->dbghd       = NULL;   /* current function stack head                       */
    thrd->dbgtl       = NULL;
    thrd->dbgjmphd    = NULL;   /* jmpbuf support                                    */
    thrd->dbgjmptl    = NULL;
    thrd->dbgfp       = stderr; /* standard debugging output                         */
	};

return thrd;
}

/* --------------------------------------------------------------------- */
/* prt_traceback: print out a routine traceback {{{2 */
void prt_traceback(void)
{
DBGTHREAD *thrd = NULL;
DBG       *dbg  = NULL;

MUTEXLOCK;
thrd= dcontext();
if(!thrd || !thrd->dbgfp) {
	thrd->dbgfp= stderr;
	}

fprintf(thrd->dbgfp,"%3d",thrd->id);
for(dbg= thrd->dbghd; dbg; dbg= dbg->nxt) fprintf(thrd->dbgfp,"%s%s", dbg->funcname, dbg->nxt? " | " : "");
fprintf(thrd->dbgfp,"\n");
MUTEXUNLOCK;
}

/* ----------------------------------------------------------------------- */
/* edbg: enter new level of function using variable arg list {{{2 */
void edbg(char *fmt,...)
{
va_list      args;
DBGTHREAD   *thrd;
DBGCMD      *d           = NULL;
static char  buf[DBGBUF];
char        *b;

MUTEXLOCK;
thrd= dcontext();

/* insure that dprintf will use NEST */
thrd->dbgnestmode= 1;

/* initialize thrd->dbgfp if null */
if(!thrd->dbgfp) {
	thrd->dbgfp= stdout;
	}

/* (1) get the function name from the fmt
 * (2) enter it into a new DBG (thrd->dbgtl)
 * (3) initialize env pointer to NULL
 */
double_link(DBG,thrd->dbghd,thrd->dbgtl,"edbg DBG");
if(fmt[0] == '%' && fmt[1] == 's') { /* assuming format of "%s" so function name is next argument */
	va_start(args,fmt);
	b= va_arg(args,char *);
	strcpy(buf,b);
	va_end(args);
	}
else {
	(void) dstptok(fmt,buf,DBGBUF,"("); /* ) */
	buf[FUNCBUF-1]  = '\0';
	}

/* sanity check on buf -- check on legality of function name */
if(!(isascii(buf[0]) || buf[0] == '_' || buf[0] == '~' || buf[0] == '$')) {
	/* the $ is for vms */
	prt_traceback();
	error(XTDIO_ERROR,"(edbg) edbg<%s> has illegal \"function\" name\n",sprt(buf));
	}
for(b= buf + 1; *b; ++b) if(!(isascii(*b) || isdigit(*b) || *b == '_' || *b == ':')) {
	prt_traceback();
	error(XTDIO_ERROR,"(edbg) edbg<%s> has illegal \"function\" name\n",sprt(buf));
	}

memset(thrd->dbgtl->funcname,0,sizeof(thrd->dbgtl->funcname));
strcpy(thrd->dbgtl->funcname,buf);
thrd->dbgtl->dbgswbuf = NULL;


/* check if routine is mentioned in a DBGCMD */
for(d= dbgcmdhd; d; d= d->nxt) {
	if(!strcmp(thrd->dbgtl->funcname,d->funcname)) break;
	}

/* process the DBGCMD */
edbg2(d);

/* set up dtst1 and dtst2 which are available to the programmer for
 * dbgswitch use
 */
dtst1= thrd->dbgtl->dtst1;
dtst2= thrd->dbgtl->dtst2;

/* enter the edbg call line into dbgswbuf.  Then, when
 * dbgswitch is used to turn debugging on for the first
 * time, it can print out the standard edbg line, too
 */
if(thrd->dbgtl->status == DBG_ON || thrd->dbgtl->status == DBG_ONDIS) {
	va_start(args,fmt);
	vsnprintf(thrd->dbgbuf,(size_t)DBGBUF,fmt,args);
	strcat(thrd->dbgbuf,"{\n");
	va_end(args);
	stralloc(thrd->dbgtl->dbgswbuf,thrd->dbgbuf,"edbg dbgswbuf");
	}
else thrd->dbgtl->dbgswbuf= (char *) NULL;

/* Report on active dbg functions.
 * Defer printing for DBG_ONDIS (let dbgswitch() do it if at all).
 */
if(thrd->dbgtl->status == DBG_ON) {
	va_start(args,fmt);
	if(showthrdid) fprintf(thrd->dbgfp,"%3d",thrd->id);
	NEST;
	vfprintf(thrd->dbgfp,fmt,args);
	fputs("{\n",thrd->dbgfp);
	++thrd->dbglvl;
	va_end(args);
	fflush(thrd->dbgfp);
	}

MUTEXUNLOCK;
}

/* --------------------------------------------------------------------- */
/* edbg2: process a DBGCMD {{{2 */
static void edbg2(DBGCMD *d)
{
DBGTHREAD *thrd;
int        reject = 0;

thrd= dcontext();


if(d) {	/* a DBGCMD has been found; use it */
	/* pre-requisite handling:  basically, a debug command is *ignored*
	 * if the pre-requisites are not satisfied (if there are any)
	 * Pre-requisites supported concern whether or not a funcname is (or is not)
	 * in the debug funcname stack.
	 */
	if(d->phd) {
		int      rejectcnt = 0;
		DBGPREQ *preq      = NULL;
		DBG     *kdbg      = NULL;

		for(preq= d->phd; preq; preq= preq->nxt) {

			for(kdbg= thrd->dbghd; kdbg; kdbg= kdbg->nxt) {
				if(!strcmp(kdbg->funcname,preq->funcname)) break;
				}

			/* insure that a !preq is *not* on the traceback DBG list */
			if(preq->select == '!') {
				if(kdbg) {	/* ah, but it is... reject dbg output */
					reject= 1;
					break;
					}
				}	/* for preq loop */

			/* if any +preq is on stack, accept */
			else if(preq->select == '+') {
				if(kdbg) break; /* found a +preq on stack, accept */
				else     ++rejectcnt;
				}

			/* all (normal) preqs must be on stack; if one isn't, reject it */
			else if(!kdbg) {
				reject= 1;
				break;
				}
			}	/* for preq loop */

		if(!reject) {

			/* for +preq: if all +preqs are rejected, then reject d */
			if(rejectcnt == d->npreq) reject= 1;
			}

		}	/* pre-requisite handling */
	}

if(d && !reject) {
	/* if d is not NULL:
	 *  the current routine is in the dhd-dtl list.  Thus,
	 *  the thrd->dbgtl->status will be altered accordingly.
	 *
	 *  thrd->dbgtl->status: there are three main states that the debugging switch
	 *                 system can be in
	 *
	 *   off            : dtst1,2 will never go non-negative, dbgswitch() cannot
	 *                    turn debugging on.  This is the normal mode of the
	 *                    debugger, strangely enough.
	 *
	 *   on but disabled: dbgswitch() can turn debugging on, but it currently
	 *                    has not done so.  What this does is allow the detail
	 *                    level to be maintained (no debug printing will occur).
	 *
	 *   on             : debugging output is now active.
	 */
	if(d->dbgout) {
		if(d->dbgout->fp) {
			thrd->dbgfp= d->dbgout->fp;
			}
		else { /* open a file.  If the trailing character is a "%", then the thread context id will be appended */
			char fname[FUNCBUF];
			strcpy(fname,d->dbgout->filename);
			if(strchr(fname,'%')) sprintf(fname+strlen(fname)-1,"%d",thrd->id);
			d->dbgout->fp= fopen(fname,"w");
			if(!d->dbgout->fp) {
				error(XTDIO_WARNING,"(edbg2) unable to open <%s>\n",fname);
				thrd->dbgfp= d->dbgout->fp= stdout;
				}
			else thrd->dbgfp= d->dbgout->fp;
			}
		if(thrd->dbgtl) thrd->dbgtl->dbgout= d->dbgout;
		}
	else if(thrd->dbgtl) thrd->dbgtl->dbgout= thrd->dbgtl->prv? thrd->dbgtl->prv->dbgout : NULL;

	/* Process the debugger command:
	 *
	 * for DBGCMD d       : current DBGCMD
	 * d->status          : DBG_OFF, DBG_ONDIS, or DBG_ON
	 *
	 *
	 * for thrd->dbgtl          : current debugging function entry
	 * status = DBG_OFF   : debugging is off
	 *        = DBG_ONDIS : debugging output disabled, dbgswitch enabled
	 *        = DBG_KEEP  : inherit normally, but set dtst1,2
	 *        = DBG_ON    : debugging output enabled,  dbgswitch enabled
	 * inherit= 0         : thrd->dbgtl should not permit subsequent inheriting
	 *        = 1         : thrd->dbgtl should     permit subsequent inheriting
	 * detail = 0...      : thrd->dbgtl's debugging detail level
	 */
	thrd->dbgtl->status = (d->status == DBG_KEEP && thrd->dbgtl && thrd->dbgtl->prv)?  thrd->dbgtl->prv->status  : d->status;
	thrd->dbgtl->detail = (d->status == DBG_ON)?    d->detail           :
	                      (d->status == DBG_ONDIS)? d->detail           :
				          (d->status == DBG_KEEP && thrd->dbgtl && thrd->dbgtl->prv)?  thrd->dbgtl->prv->detail  : 0;
	thrd->dbgtl->inherit= (d->status != DBG_KEEP)?  d->inherit          :
	                      (thrd->dbgtl && thrd->dbgtl->prv)?    thrd->dbgtl->prv->inherit : 0;
	thrd->dbgtl->dtst1  = (d->dtst1 > DBG_DTSTOFF)? d->dtst1            :
	                      thrd->dbgtl->prv?               thrd->dbgtl->prv->dtst1   : DBG_DTSTOFF;
	thrd->dbgtl->dtst2  = (d->dtst2 > DBG_DTSTOFF)? d->dtst2            :
	                      thrd->dbgtl->prv?               thrd->dbgtl->prv->dtst2   : DBG_DTSTOFF;
	thrd->dbgtl->dbgdfl = d->dbgdfl;

	}	/* if(d) : processed debugger command */

else if(d && reject) {
	thrd->dbgtl->status = DBG_OFF;
	thrd->dbgtl->inherit= 0;
	thrd->dbgtl->dtst1  = thrd->dbgtl->dtst2= DBG_DTSTOFF;
	thrd->dbgtl->dbgout = thrd->dbgtl->prv? thrd->dbgtl->prv->dbgout : NULL;
	thrd->dbgtl->dbgdfl = thrd->dbgtl->prv? thrd->dbgtl->prv->dbgdfl : 0;
	}

/* inheritance mode: funcname not in dhd-dtl list.  Infer active/inactive
 * etc based on preceding function's dbg info
 */
else if(thrd && thrd->dbgtl && thrd->dbgtl->prv && thrd->dbgtl->prv->inherit) {
	thrd->dbgtl->status = (thrd->dbgtl->prv->status == DBG_ONDIS)? DBG_OFF : thrd->dbgtl->prv->status;
	thrd->dbgtl->detail = thrd->dbgtl->prv->detail;
	thrd->dbgtl->inherit= thrd->dbgtl->prv->inherit;
	thrd->dbgtl->dtst1  = thrd->dbgtl->prv->dtst1;
	thrd->dbgtl->dtst2  = thrd->dbgtl->prv->dtst2;
	thrd->dbgtl->dbgout = thrd->dbgtl->prv->dbgout;
	thrd->dbgtl->dbgdfl = thrd->dbgtl->prv->dbgdfl;
	}

/* funcname not in dbgstr & inherit is off, so shut thrd->dbgtl->status off, too */
else if(thrd && thrd->dbgtl) {
	thrd->dbgtl->status = DBG_OFF;
	thrd->dbgtl->inherit= 0;
	thrd->dbgtl->dtst1  = thrd->dbgtl->dtst2= DBG_DTSTOFF;
	thrd->dbgtl->dbgout = thrd->dbgtl->prv? thrd->dbgtl->prv->dbgout : NULL;
	thrd->dbgtl->dbgdfl = 0;
	}
}

/* ----------------------------------------------------------------------- */
/* rdbg: return from level of function using variable arg lists {{{2 */
void rdbg(char *fmt,...)
{
va_list    args;
DBGTHREAD *thrd;
DBG       *dbgprv = NULL;

MUTEXLOCK;
thrd= dcontext();


/* insure that dprintf will use NEST */
thrd->dbgnestmode= 1;

/* sanity checks */
if(!thrd->dbghd) error(XTDIO_ERROR, "(rdbg) attempt to rdbg before initdbg\n");
if(!thrd->dbgtl) error(XTDIO_SEVERE,"(rdbg) thrd->dbghd exists but thrd->dbgtl doesn't\n");
if(!fmt)   error(XTDIO_ERROR, "(rdbg) fmt is null\n");

if(!thrd->dbgfp) thrd->dbgfp= stdout; /* Anti-Murphy Protector */

/* print out return message for active debugging */
switch(thrd->dbgtl->status) {

case DBG_ONDIS:
	if(thrd->dbgtl->dbgswbuf) break;
	/* fall through, dbgswitch used to emit edbg stuff */

case DBG_ON: /* print out return message */
	va_start(args,fmt);

	if(showthrdid) fprintf(thrd->dbgfp,"%3d",thrd->id);
	NEST;
	fprintf(thrd->dbgfp,"return ");
	vfprintf(thrd->dbgfp,fmt,args);
	if(drcdbgline > 0) fprintf(thrd->dbgfp," }~%d\n",drcdbgline);
	else               fprintf(thrd->dbgfp," }\n");
	va_end(args);
	fflush(thrd->dbgfp);
	if(thrd->dbglvl > 0) --thrd->dbglvl;
	break;

case DBG_KEEP:	/* leave unchanged */
	break;

case DBG_OFF:
	dstptok(stpblk(fmt),thrd->dbgbuf,DBGBUF," \t("); /* ) */
	break;

default:
	error(XTDIO_ERROR,"(rdbg) thrd->dbgtl->status=%d unknown\n",thrd->dbgtl->status);
	exit(1);
	}

/* edbg/rdbg func name mismatch check */
if(fmt[0] == '%' && fmt[1] == 's') { /* assuming format of "%s..." so function name is next argument */
	char *b;
	va_start(args,fmt);
	b= va_arg(args,char *);
	strcpy(thrd->dbgbuf,b);
	va_end(args);
	}
else (void) dstptok(stpblk(fmt),thrd->dbgbuf,DBGBUF," \t(");	 /* ) */
if(strcmp(thrd->dbgbuf,thrd->dbgtl->funcname)) {
	prt_traceback();
	error(XTDIO_ERROR,"(rdbg) edbg<%s> rdbg<%s> mismatch\n",
	  sprt(thrd->dbgtl->funcname),
	  sprt(thrd->dbgbuf));
	}

/* handle return to prior thrd->dbgfp */
dbgprv= thrd->dbgtl->prv;

/* handle debug file output redirection */
if(dbgprv) {
	if(thrd->dbgtl->dbgout != dbgprv->dbgout) {
		thrd->dbgfp= dbgprv->dbgout? dbgprv->dbgout->fp : stdout;
		if(!thrd->dbgfp) thrd->dbgfp= stdout;
		}
	}
else {
	thrd->dbgfp= stdout;
	}

/* restore programmer accessable dtst1 and dtst2 values */
dtst1= dbgprv? dbgprv->dtst1 : DBG_DTSTOFF;
dtst2= dbgprv? dbgprv->dtst2 : DBG_DTSTOFF;

/* free up resources if rdbg from main is taken */
if(thrd->dbgtl == thrd->dbghd) {
	DBGCMD *d       = NULL;
	DBGCMD *dnxt    = NULL;
	DBGOUT *dout    = NULL;
	DBGOUT *doutnxt = NULL;


	for(d= dbgcmdhd; d; d= dnxt) {
		dnxt= d->nxt;
		if(d->phd) {
			DBGPREQ *preq    = NULL;
			DBGPREQ *preqnxt = NULL;
			for(preq= d->phd; preq; preq= preqnxt) {
				preqnxt= preq->nxt;
				free((char *) preq);
				}
			}
		free((char *) d);
		}
	dbgcmdhd= dbgcmdtl= NULL;

	/* close up DBGOUT file handles */
	for(dout= doutnxt= dbgouthd; dout; dout= doutnxt) {
		doutnxt= dout->nxt;
		if(dout->filename) free(dout->filename);
		if(dout->fp)       fclose(dout->fp);
		dout->filename = NULL;
		dout->fp       = NULL;
		dout->nxt      = dout->prv = NULL;
		free((char *) dout);
		}
	dbgouthd    = dbgouttl = NULL;
	thrd->dbgfp = stderr;
	}

else {	/* clean up the return stacks */

	/* pop the longjmp stack */
	while(thrd->dbgjmptl) {
		if(thrd->dbgtl != thrd->dbgjmptl->dbg) break;
		delete_double_link(DBGJMPBUF,thrd->dbgjmptl,thrd->dbgjmphd,thrd->dbgjmptl);
		}

	/* pop the DBG stack */
	if(thrd->dbgtl->dbgswbuf) {
		free(thrd->dbgtl->dbgswbuf);
		thrd->dbgtl->dbgswbuf= NULL;
		}
	delete_double_link(DBG,thrd->dbgtl,thrd->dbghd,thrd->dbgtl);
	}

/* insure that as much output that can be gotten to the output gets
 * there!
 */
fflush(thrd->dbgfp);

#if defined(__WIN32__) || defined(__CYGWIN__)
/* release critical section resources if return main 0 */
if(fmt[0] == 'm' && fmt[1] == 'a' && fmt[2] == 'i' && fmt[3] == 'n' && (fmt[4] == '\0' || !isalnum(fmt[4]))) {
	DeleteCriticalSection(&dbgmutex);
	}
#endif

MUTEXUNLOCK;
}

/* ----------------------------------------------------------------------- */
/* dprintf: used to print nested levels of debug info under dlist control {{{2 
 *  Returns +: if debugging printing went out
 *          0: otherwise
 *          -: use |detail|, but suppress leading |||s
 */
int dprintf(int detail,char *fmt,...)
{
DBGTHREAD *thrd;
va_list    args;

MUTEXLOCK;
thrd= dcontext();


if(!fmt) {
	thrd->dbgnestmode= 1;
	MUTEXUNLOCK;
	return 0;
	}

if(!thrd->dbghd) {
	error(XTDIO_WARNING,"(dprintf) attempt to dprintf before edbg (fmt<%s>)\n",sprt(fmt));
	prt_traceback();
	MUTEXUNLOCK;
	return 0;
	}

/* if the current debug mode is DBG_ON */
if(thrd->dbgtl->status == DBG_ON) {
	char *fmt0= fmt;

    /* detail >= 0:  current debugging detail level  >= this dprintf's detail level => print it
     * detail <  0: |current debugging detail level| == this dprintf's detail level => print it
	 */
	if((detail < 0 && thrd->dbgtl->detail == -detail) || (detail >= 0 && thrd->dbgtl->detail >= detail)) {
		int slen;

		/* put out leading newlines with no NEST */
		for(; fmt && *fmt == '\n'; ++fmt) fputc('\n',thrd->dbgfp);

		if(fmt && *fmt) {	/* if there's more format left... */
			va_start(args,fmt);
	
			if(thrd->dbgnestmode) {
				if(detail >= 0) {
					if(showthrdid) fprintf(thrd->dbgfp,"%3d",thrd->id);
					NEST;
					}
				if(dbg_Nflag) fprintf(thrd->dbgfp,"%s: ",thrd->dbgtl->funcname);
				}
			vsnprintf(thrd->dbgbuf,(size_t)DBGBUF,fmt,args);
			slen= strlen(thrd->dbgbuf);
			if(thrd->dbgbuf[slen-1] == '\n' && thrd->dbgtl->dbgdfl) {
				static char *fmt2     = NULL;
				static size_t fmt2len = 0;
				if(slen >= fmt2len) {
					fmt2len = slen + slen + 10;
					fmt2    = (char *) calloc(fmt2len,sizeof(char));
					}
				strcpy(fmt2,thrd->dbgbuf);
				fmt2[slen-1]= '\0'; /* strip off trailing newline */
				fputs(fmt2,thrd->dbgfp);
				if(drcdbgline > 0) fprintf(thrd->dbgfp,"~%d\n",drcdbgline);
				else               fprintf(thrd->dbgfp,"\n");
				}
			else fputs(thrd->dbgbuf,thrd->dbgfp);

			/* determine next dprintf's thrd->dbgnestmode (allows one to string
			 * multiple dprintf's together on one line)
			 */
			thrd->dbgnestmode= (*fmt0 == '\n')? 1 : (slen && thrd->dbgbuf[slen - 1] == '\n');

			va_end(args);
			}
		else thrd->dbgnestmode= 1;

		fflush(thrd->dbgfp);
		MUTEXUNLOCK;
		return thrd->dbgtl->detail;
		}

	fflush(thrd->dbgfp);
	}

MUTEXUNLOCK;
return 0;
}

/* --------------------------------------------------------------------- */
/* dbgchkswitch: this function checks if dtst1 is DBG_DTSTOFF before trying dbgswitch() {{{2
 *               dbgswitch() will turn debugging on or off.
 *               dbgchkswitch() will check dtst1 to see if it may call dbgswitch()
 */
void dbgchkswitch(int on)
{

if(dtst1 != DBG_DTSTOFF || dtst2 != DBG_DTSTOFF) dbgswitch(on);

}

/* ----------------------------------------------------------------------- */
/* dbgswitch: turn thrd->dbgtl->status to the specified on state {{{2
 *            if on > 0: detail level set to on
 *            if on < 0: detail level left as is
 *            if on = 0: debugging status set to off
 */
int dbgswitch(int on)
{
DBGTHREAD *thrd;
int        rtrn;

MUTEXLOCK;
thrd= dcontext();


if(on) {
	if(thrd->dbgtl->status == DBG_ONDIS) {	/* enable debugging output */
		thrd->dbgtl->status= DBG_ON;
		thrd->dbgtl->detail= (on > 0)? on : thrd->dbgtl->detail;
		if(thrd->dbgtl->dbgswbuf) { /* emit the associated edbg call line */
			if(showthrdid) fprintf(thrd->dbgfp,"%3d",thrd->id);
			NEST;
			fputs(thrd->dbgtl->dbgswbuf,thrd->dbgfp);
			++thrd->dbglvl;
			free(thrd->dbgtl->dbgswbuf);
			thrd->dbgtl->dbgswbuf= (char *) NULL;
			}
		}
	else if(thrd->dbgtl->status == DBG_ON) thrd->dbgtl->detail= on;
	else if(thrd->dbgtl->status == DBG_OFF) {
		error(XTDIO_WARNING,"debugging is off for function<%s> (did you want it to be \"ondis\"?)\n",
		  sprt(thrd->dbgtl->funcname));
		}
	}

else if(thrd->dbgtl->status == DBG_ON) {	/* disable debugging output	*/
	thrd->dbgtl->status= DBG_ONDIS;		/* its a lot like DBG_OFF	*/
	}

rtrn= dbgtst(on);

MUTEXUNLOCK;
return rtrn;
}

/* ----------------------------------------------------------------------- */
/* initdbg: this function process command line arguments {{{2
 *    pgmname -@ dbgfile [ dtst1 [dtst2]]
 *
 *    Input:                   Arguments:                      Returns in *argc
 *    ----------               -----------------------         ----------------
 *     *argc > 0   n == *argc  pgmname [arg1 ... arg_n] -@...      n+1
 *     *argc < 0   n == -*argc [arg1 ... arg_n] -@...               n
 *
 *    Note that this routine does *not* modify argv[...]!
 */
void initdbg(
  int   *argc,
  char **argv)
{
DBGTHREAD   *thrd;
char        *b              = NULL;
int          iargc;
int          idbg;
int          locbuflen      = 0;
int          newargc;
int          use_default    = 0;
int          use_dflt_lvl   = 1;
static char  locbuf[DBGBUF];

thrd= dcontext();


iargc= *argc;
if(*argc < 0) iargc= -iargc;
thrd->dbgfp= stdout;

/* enter "main" into debugging system */
sprintf(locbuf,"main(argc=%d,argv<",iargc);
for(idbg= locbuflen= 0; idbg < *argc; ++idbg) {
	/* prevent locbuf overflow */
	locbuflen+= strlen(argv[idbg]) + 1;
	if(locbuflen > DBGBUF-3) break;

	/* concatenate command line into locbuf */
	b= strpbrk(argv[idbg]," \t");
	if(b) strcat(locbuf,"\"");
	strcat(locbuf,argv[idbg]);
	if(b) strcat(locbuf,"\"");
	strcat(locbuf,(idbg == *argc-1)? ">)" : " ");
	}

/* initialize debugging system to off */
dtst1= dtst2= DBG_DTSTOFF;

/* find a debugging list:  pgmname ... -@ ... (dbglist) ...  */
if(*argc > 0) --iargc, ++argv, newargc= 1;
else          newargc= 0;
for(; iargc; --iargc, ++argv, ++newargc) {
	if((*argv)[0] == '-' && (*argv)[1] == '@') {
		use_default= 1;
		if((*argv)[2] == '@') use_default= 2;
		if(isdigit((*argv)[use_default+1]))  use_dflt_lvl= atoi(*argv + use_default + 1);
		break;
		}
	}

/* handle embedded "%"s in the locbuf to prevent edbg() from attempting
 * to interpret them as format codes
 */
for(b= locbuf; b && *b; ++b) {
	b= strchr(b,'%');
	if(!b) break;
	memmove(b+1,b,(int) (locbuf + DBGBUF - b));
	*b= '%';
	++b;
	}

/* if iargc == 0: did not find a -@ in argv list of arguments */
if(!iargc) {
	edbg(locbuf);
	return;
	}

/* modify user's argc so it doesn't process debug's information
 * set up default dbg line (on main 1)
 */
*argc       = newargc;
argv[0][0]  = '\0';
argv[0]     = NULL;

/* initialize debugging system */
for(--iargc, ++argv; iargc; --iargc, ++argv) {
	if(**argv == '-' || **argv == '+' || isdigit(**argv)) break;

	/* if "on" or "off", *or* has whitespace, then
	 * interpret the string as a command for initdbg3()
	 */
	if(!strcmp(*argv,"thrd")) initdbg3(*argv);
	else {
		for(b= *argv; *b; ++b) if(isspace(*b)) break;

		if(*b) {
			/* has whitespace, parse the command */
			initdbg3(*argv);
			use_default= 0;
			}
		else {
			/* treat as filename which presumably has dbg info */
			initdbg2(*argv);
			use_default= 0;
			}
		}
	}

/* set up default if use_default is true */
if     (use_default == 1) initdbg3(strprintf("on main %d",use_dflt_lvl));
else if(use_default == 2) initdbg3(strprintf("on main %d > tmp",use_dflt_lvl));

if(iargc) {	/* read in dtst1				*/
	sscanf(*argv,"%d",&dtst1);
	if(dtst1 < 0) dtst1         = DBG_DTSTOFF;
	else          thrd->dbghd->status = DBG_ONDIS;
	--iargc; ++argv;
	}
else dtst1= DBG_DTSTOFF;

if(iargc) {	/* read in dtst2				*/
	sscanf(*argv,"%d",&dtst2);
	if(dtst2 < 0) dtst2         = DBG_DTSTOFF;
	else          thrd->dbghd->status = DBG_ONDIS;
	}
else dtst2= DBG_DTSTOFF;

/* report on main(...) */
edbg(locbuf);

}

/* ----------------------------------------------------------------------- */
/* initdbg2: this routine initializes debugging {{{2
 *
 * static DBGSTR {
 * 	int    on;		// turn dbg on(off)
 * 	char  *fname;	// function name
 * 	int    detail;	// detail on/off
 * 	char **preq;	// pre-requisite list
 *	int    npreq;	// number of pre-requisites
 * 	} *dhd=NULL,*dtl=NULL;
 *
 */
void initdbg2(char *dbgfile)
{
DBGTHREAD   *thrd;
static char  buf[DBGBUF];
FILE        *fp;

thrd= dcontext();


/* open <dbgfile> */
fp= fopen(dbgfile,"r");
if(!fp) { /* unable to open <dbgfile> */
	error(XTDIO_ERROR,"(initdbg2) unable to open <%s>\n",dbgfile);
	exit(1);
	}

/* read <dbgfile> */
while(fgets(buf,DBGBUF,fp)) initdbg3(buf);

/* close dbgfile */
fclose(fp);

}

/* ----------------------------------------------------------------------- */
/* initdbg3: this init dbg routine processes a text buffer {{{2 */
void initdbg3(char *buf)
{
DBGTHREAD *thrd;
char      *b;
int        detail;
DBGCMD    *d;
char       fnamebuf[DBGBUF];        /* function name buffer */
char       modebuf[DBGBUF];         /* on/off/del           */

MUTEXLOCK;
thrd = dcontext();
b    = NULL;
d    = NULL;


/* elide comments and ignore blank lines */
b= (char *) strchr(buf,'#');
if(b) *b= '\0';
srmtrblk(buf);

if(buf[0] == '\0') {	/* ignore comment/blank lines */
	MUTEXUNLOCK;
	return;
	}

/* handle on/off/del and function name
 * modebuf has on/off/del/keep
 * fnamebuf has funcname (or *)
 */
sscanf(buf,"%s %s",modebuf,fnamebuf);
fnamebuf[FUNCBUF-1]= '\0';/* insure that there are no more than FUNCBUF chars */

/* check if function name is already in dbgstr */
for(d= dbgcmdhd; d; d= d->nxt) if(!strcmp(fnamebuf,d->funcname)) break;

/* check if deletion has been called for */
if(!strcmp(modebuf,"del")) {

	/* delete all dbg info for all routines if fnamebuf is "*" */
	if(!strcmp(fnamebuf,"*")) {
		DBGCMD *dnxt=NULL;

		for(d= dbgcmdhd; d; d= dnxt) {
			dnxt= d->nxt;

			/* delete pre-requisite info */
			if(d->phd) {
				DBGPREQ *p    = NULL;
				DBGPREQ *pnxt = NULL;

				for(p= d->phd; p; p= pnxt) {
					pnxt= p->nxt;
					delete_double_link(DBGPREQ,p,d->phd,d->ptl);
					}
				}
			delete_double_link(DBGCMD,d,dbgcmdhd,dbgcmdtl);
			}
		MUTEXUNLOCK;
		return;
		}

	if(!d) {	/* can't delete an unknown routine */
		error(XTDIO_WARNING,"(initdbg3) <%s> not in dbgcmd data base, can't delete\n",fnamebuf);
		MUTEXUNLOCK;
		return;
		}

	/* delete pre-requisite info */
	if(d->phd) {
		DBGPREQ *p    = NULL;
		DBGPREQ *pnxt = NULL;

		for(p= d->phd; p; p= pnxt) {
			pnxt= p->nxt;
			delete_double_link(DBGPREQ,p,d->phd,d->ptl);
			}
		}

	/* delete d from dbg linked list */
	delete_double_link(DBGCMD,d,dbgcmdhd,dbgcmdtl);
	MUTEXUNLOCK;
	return;
	}

/* allocate and link a new DBGCMD */
if(!d) {
	double_link(DBGCMD,dbgcmdhd,dbgcmdtl,"initdbg3 DBGCMD");
	d         = dbgcmdtl;
	memset(d->funcname,0,sizeof(d->funcname));
	strcpy(d->funcname,fnamebuf);
	d->phd    = d->ptl = NULL;
	d->dbgout = NULL;
	}

else {	/* re-use a DBGCMD (ie. overwrite it) */
	if(d->phd) {
		DBGPREQ *p    = NULL;
		DBGPREQ *pnxt = NULL;

		for(p= d->phd; p; p= pnxt) {
			pnxt= p->nxt;
			delete_double_link(DBGPREQ,p,d->phd,d->ptl);
			}
		}
	d->dbgout= NULL;
	}

/* set up d->status:
 *   on   : when funcname encountered, debugger goes on  till function returns
 *   off  : when funcname encountered, debugger goes off till function returns
 *   ondis: on but disabled -- only goes on if a dbgswitch() does it
 *   keep : don't change on/off status (used to feed dtst1,2 values)
 *   thrd : precede debugging output with thread identifier number
 */
if     (modebuf[0] == '1' || !strcmp(modebuf,"on"))    d->status= DBG_ON;
else if(modebuf[0] == '2' || !strcmp(modebuf,"ondis")) d->status= DBG_ONDIS;
else if(modebuf[0] == '3' || !strcmp(modebuf,"keep"))  d->status= DBG_KEEP;
else if(!strcmp(modebuf,"thrd")) {
	showthrdid= !showthrdid;
	}
else                                                   d->status= DBG_OFF;

/* standard defaults for number of pre-reqs and dtsts */
d->phd     = d->ptl   = NULL;
d->dtst1   = d->dtst2 = DBG_DTSTOFF;
d->dbgdfl  = (d->prv)? d->prv->dbgdfl : 1; /* initialize default dfl to inheritance */
d->inherit = 1;
d->detail  = 0;

/* if the dbgcmd is an "off", no point in looking further! */
if(d->status != DBG_OFF) { /* parse rest of debugging command */


	/* handle detail level:
 	 *  b is set to point past the first two strings and any
 	 *  white space (ie. skip past "<on/off/del> <funcname>[whitespace]" )
 	 */
	b= stpnxt(buf,"%s %s%b");

	/* look for '*' : don't inherit mode */
	if(*b && *b == '*' && isspace(*(b+1))) {
		b      = stpblk(b+2);
		d->inherit= 0;
		}


	/* pick up detail level for d->status ON/ONDIS */
	if(*b && (d->status == DBG_ON || d->status == DBG_ONDIS)) {

		if(isdigit(*b)) {
			sscanf(b,"%d",&detail);
			b= stpnxt(b,"%d%b");
			}
		else detail= 1;	/* default detail level is 1 */
		if(detail < 0) error(XTDIO_ERROR,"(initdbg3) %s has detail %d < 0!\n",d->funcname,detail);
		d->detail = detail;
		}
	}
else {
	b= stpnxt(buf,"%s %s%b");
	if(*b) {
		d->status= DBG_ONDIS;
		}
	}


/* read in [:nodfl] [:ln [ :dtst1 [ :dtst2 ]]] (optional!) */
if(*b) {
	if(*b == ':') {
		char *bb;
		bb= stpblk(b+1);
		if(bb[0] == 'n' && bb[1] == 'o' && bb[2] == 'd' && bb[3] == 'f' && bb[4] == 'l') {
			d->dbgdfl = 0;
			b         = stpblk(bb+3);
			}
		}

	/* look for dtst1 */
	if(*b == ':') {
		b= stpblk(b+1);
		sscanf(b,"%d",&d->dtst1);
		if(d->dtst1 < 0) d->dtst1= DBG_DTSTOFF;
		b= stpnxt(b,"%d%b");

		if(*b == ':') {
			b= stpblk(b+1);
			sscanf(b,"%d",&d->dtst2);
			if(d->dtst2 < 0) d->dtst2= DBG_DTSTOFF;
			b= stpnxt(b,"%d%b");
			}
		}
	/* if a :# is present, set status to DBG_ONDIS (ie. use dbgswitch)
	 *  I commented the following line out because  ondis func lvl
	 *  wasn't working with it. 10/06/06
	 */
/*    if(d->status == DBG_ONDIS && !(d->dtst1 == DBG_DTSTOFF || d->dtst2 == DBG_DTSTOFF)) d->status= DBG_OFF;*/
	}


/* handle pre-requisites */
d->phd  = d->ptl= NULL;
d->npreq= 0;
for(; *b && *b != '>'; b= stpnxt(b,"%s%b")) {
	d->npreq++;
	sscanf(b,"%s",modebuf);

	/* sanity check */
	if(!isalpha(modebuf[0]) && modebuf[0] != '_' && modebuf[0] != '$' && modebuf[0] != '!' && modebuf[1] != '+') {
		error(XTDIO_WARNING,"(initdbg3) bad pre-requisite name<%s>\n",sprt(modebuf));
		continue;
		}

	double_link(DBGPREQ,d->phd,d->ptl,"initdbg3 DBGPREQ");
	if(isalpha(modebuf[0])) {
		modebuf[FUNCBUF-1]= '\0';
		d->ptl->select= ' ';
		memset(d->ptl->funcname,0,sizeof(d->ptl->funcname));
		strcpy(d->ptl->funcname,modebuf);
		}
	else {
		modebuf[FUNCBUF]= '\0';
		d->ptl->select  = modebuf[0];
		memset(d->ptl->funcname,0,sizeof(d->ptl->funcname));
		strcpy(d->ptl->funcname,modebuf+1);
		}
	}


/* set up DBGOUT */
d->dbgout= NULL;
if(*b == '>') {
	DBGOUT *dbgout=NULL;

	/* get filename into modebuf */
	sscanf(b+1,"%s",modebuf);
	b= stpnxt(b+1,"%s%b");

	/* search for dbgout filename in dbgouthd-tl list */
	for(dbgout= dbgouthd; dbgout; dbgout= dbgout->nxt) {
		if(!strcmp(dbgout->filename,modebuf)) break;
		}

	if(dbgout) d->dbgout= dbgout;
	else {	/* set up a new DBGOUT */
		double_link(DBGOUT,dbgouthd,dbgouttl,"initdbg3 DBGOUT");
		stralloc(dbgouttl->filename,modebuf,"initdbg3 DBGOUT filename");
		d->dbgout   = dbgouttl;
		dbgouttl->fp= NULL;
		}
	}

/* if the new debugging command's function and the current function
 * we're in have the same name, then modify debugging status.  Note that
 * edbg, not edbg2, generates a new DBG; edbg2 just interprets the
 * DBGCMD and modifies the current debugging mode.
 */
if(thrd->dbgtl && !strcmp(d->funcname,thrd->dbgtl->funcname)) edbg2(d);

MUTEXUNLOCK;
}

/* ----------------------------------------------------------------------- */
/* prt_dbgstr: print out debugging command info {{{2 */
void prt_dbgstr(char *fmt,...)
{
va_list    args;
DBGTHREAD *thrd;
DBGCMD    *d    = NULL;

MUTEXLOCK
thrd= dcontext();

/* print out fmt information */
va_start(args,fmt);

vfprintf(thrd->dbgfp,fmt,args);
fprintf(thrd->dbgfp,"Current Debugging Table\n");

/* print out debug command info */
for(d= dbgcmdhd; d; d= d->nxt) {
	if(!isalpha(fmt[0])) vfprintf(thrd->dbgfp,fmt,args);

	fprintf(thrd->dbgfp,"%s %s %s detail=%d dtst=%d,%d",
	  sprt(d->funcname),
	  (d->status == DBG_OFF)?   "off"   :
	  (d->status == DBG_ONDIS)? "ondis" :
	  (d->status == DBG_KEEP)?  "keep"  :
	  (d->status == DBG_ON)?    "on"    : "???",
	  d->inherit? "inherit"  : "*",
	  d->detail,
	  d->dtst1,
	  d->dtst2);

	if(d->phd) {	/* print out any pre-requisites */
		DBGPREQ *p;

		fputs(" preq[ ",thrd->dbgfp);
		for(p= d->phd; p; p= p->nxt)
		  fprintf(thrd->dbgfp," %s%s",cprt(p->select),p->funcname);
		fputs("]",thrd->dbgfp);
		}
	if(d->dbgout) fprintf(thrd->dbgfp," >%s",d->dbgout->filename);
	fputc('\n',thrd->dbgfp);
	}

/* terminate variable arguments */
va_end(args);
fflush(thrd->dbgfp);
MUTEXUNLOCK
}

/* ----------------------------------------------------------------------- */
/* dsetjmp: this function sets up a setjmp tag for debugging {{{2 */
void dsetjmp(void *env)
{
DBGJMPBUF *jb    = NULL;
DBGJMPBUF *jbnxt = NULL;
DBGTHREAD *thrd;

MUTEXLOCK;
thrd= dcontext();


if(!thrd->dbghd) error(XTDIO_WARNING,"(dsetjmp) attempt to dsetjmp prior to use of edbg\n");

/* search for prior use of env */
for(jb= thrd->dbgjmphd; jb; jb= jbnxt) {
	jbnxt= jb->nxt;
	if(jb->env == env) {	/* de-link and de-allocate */
		/* sanity preparation */
		jb->env= NULL;
		jb->dbg= NULL;

		/* delink and de-allocate the DBGJMPBUF */
		delete_double_link(DBGJMPBUF,jb,thrd->dbgjmphd,thrd->dbgjmptl);
		break;
		}
	}

/* allocate, link, and initialize a new jmpbuf_str */
double_link(DBGJMPBUF,thrd->dbgjmphd,thrd->dbgjmptl,"dsetjmp");
thrd->dbgjmptl->env= env;		/* save setjmp environment	*/
thrd->dbgjmptl->dbg= thrd->dbgtl;	/* save associated DBG		*/

MUTEXUNLOCK;
}

/* ----------------------------------------------------------------------- */
/* dlongjmp: multi-level return (longjmp) {{{2 */
void dlongjmp(void *env,char *fmt,...)
{
va_list    args;
DBG       *dbg    = NULL;
DBG       *dbgprv = NULL;
DBGJMPBUF *jb     = NULL;
DBGTHREAD *thrd;

MUTEXLOCK;
thrd= dcontext();


/* insure that dprintf will use NEST */
thrd->dbgnestmode= 1;

if(!thrd->dbghd) {
	error(XTDIO_WARNING,"(dlongjmp) attempt to dlongjmp before edbg\n");

	MUTEXUNLOCK;
	return;
	}

/* edbg/rdbg func name mismatch check if fmt provides one */
if(fmt[0]) {
	/* extract function name from format string */
	dstptok(stpblk(fmt),thrd->dbgbuf,DBGBUF," \t(");

	/* should match current dbg's function name */
	if(strcmp(thrd->dbgbuf,thrd->dbgtl->funcname)) {
		prt_traceback();
		error(XTDIO_ERROR,"(dlongjmp) edbg<%s> rdbg<%s> mismatch\n",thrd->dbgtl->funcname,thrd->dbgbuf);
		}
	}

/* look for target environment */
for(jb= thrd->dbgjmphd; jb; jb= jb->nxt) {
	if(env == jb->env) break;
	}

/* sanity check on target environment: it'd better be there! */
if(!jb) {
	prt_traceback();
	error(XTDIO_ERROR,"(dlongjmp) setjmp/longjmp mismatch in <%s>\n",thrd->dbgtl->funcname);
	}

/* print out return message for active debugging */
if(thrd->dbgtl->status == DBG_ON) {
	if(showthrdid) fprintf(thrd->dbgfp,"%3d",thrd->id);
	NEST;

	/* longjmp taken to point inside current routine */
	if(jb->dbg == thrd->dbgtl) {
		if(fmt[0]) {
			va_start(args,fmt);
			fprintf(thrd->dbgfp,"longjmp within %s : ",thrd->dbgtl->funcname);
			vfprintf(thrd->dbgfp,fmt,args);
			va_end(args);
			}
		else fprintf(thrd->dbgfp,"longjmp within %s",thrd->dbgtl->funcname);
		fputs("}\n",thrd->dbgfp);

		fflush(thrd->dbgfp);

		MUTEXUNLOCK;
		return;
		}

	/* longjmp taken to routine on calling stack (usual case) */
	else {
		if(fmt[0]) {
			va_start(args,fmt);
			fprintf(thrd->dbgfp,"longjmp return %s : ",thrd->dbgtl->funcname);
			vfprintf(thrd->dbgfp,fmt,args);
			va_end(args);
			}
		else fprintf(thrd->dbgfp,"longjmp return %s",thrd->dbgtl->funcname);

		fputs("}\n",thrd->dbgfp);
		delete_double_link(DBG,thrd->dbgtl,thrd->dbghd,thrd->dbgtl);
		--thrd->dbglvl;
		}

	}

/* pop the rest of the return stack */
for(dbg= thrd->dbgtl, dbgprv= NULL; dbg; dbg= dbgprv) {
	dbgprv= dbg->prv;


	if(dbg == jb->dbg) break;

	/* print out return message for active debugging */
	if(dbg->status == DBG_ON) {
		if(showthrdid) fprintf(thrd->dbgfp,"%3d",thrd->id);
		NEST;
		fprintf(thrd->dbgfp,"longjmp return %s: skipped}\n",dbg->funcname);
		}
	--thrd->dbglvl;
	delete_double_link(DBG,dbg,thrd->dbghd,thrd->dbgtl);
	}

fflush(thrd->dbgfp);

MUTEXUNLOCK;
}

/* ----------------------------------------------------------------------- */
/* dbgtst: if a dprintf at the given detail level would print, this {{{2
 * function will return 1, otherwise 0
 */
int dbgtst(int detail)
{
DBGTHREAD *thrd;

MUTEXLOCK
thrd= dcontext();
MUTEXUNLOCK

if(!thrd->dbghd) return 0;
return thrd->dbgtl->status == DBG_ON && thrd->dbgtl->detail >= detail;
}

/* ----------------------------------------------------------------------- */
/* prt_debugstate: this function prints out the function name and the {{{2
 * debugging state for the current function
 */
void prt_debugstate(char *fmt,...)
{
va_list    args;
DBGTHREAD *thrd;

MUTEXLOCK;
thrd= dcontext();

va_start(args,fmt);

vfprintf(thrd->dbgfp,fmt,args);
fprintf(thrd->dbgfp,"%7s: %s %s dtl=%d dtst=%d,%d\n",
  thrd->dbgtl->funcname,
  thrd->dbgtl->status?  "active"  : "inactive",
  thrd->dbgtl->inherit? "inherit" : "no-inherit",
  thrd->dbgtl->detail,
  thrd->dbgtl->dtst1,
  thrd->dbgtl->dtst2);

va_end(args);
MUTEXUNLOCK;
}

/* ----------------------------------------------------------------------- */
/* dstptok: gets a token from a string. {{{2
 *
 * Parameters:
 *	s     : input string- if NULL, then last string used
 *	tok   : returned token
 *	toklen: max length of a token
 *	brk   : string of not-to-be-included in a token characters (break string)
 *
 * Returns a pointer to the break character which terminates the token,
 * and the token itself in <tok>.
 */
static char *dstptok(char *s,char *tok,int toklen,char *brk)
{
static char *ss = NULL;
char        *p;

if(!brk || !tok) return (ss= (char *) NULL); /* sanity checks                                        */
*tok= '\0';                                  /* initialize token to null string                      */

if(!s) s= ss;                                /* if s is NULL, set s to ss                            */

if(s) {                                      /* if s is not NULL, look for first non-break char in s */
	do {
		if(!*s) return (ss= (char *) NULL);
		for(p= brk; *p && *s != *p; ++p) ;
		if(*p) ++s;
		} while(*p);
	}
else return (ss= (char *) NULL);	/* no string available			*/

/* advance s to point to a break-string character
 *  Note that token>1 test; that insures that there
 *  will be at least one byte available in the token
 *  buffer for the terminating null byte.
 */
for(; toklen > 1 && *s; ++s, --toklen, ++tok) {

	/* check if *s is a break string character						*/
	for(p= brk; *p && *s != *p; ++p) ;

	/* when *s is in break string, then *s == *p					*/
	if(*s == *p) {
		*tok= '\0';		/* terminate tok correctly					*/
		ss  = s;		/* save s pointer for next dstptok call		*/
		return s;
		}
	*tok= *s;			/* store non-break-string char in tok		*/
	}

*tok= '\0';				/* terminate tok correctly					*/
ss  = s;				/* save s pointer for next dstptok call		*/

return s;
}

/* --------------------------------------------------------------------- */
/* dbgfpget: returns current thrd->dbgfp FILE * handle {{{2 */
FILE *dbgfpget(void)
{
DBGTHREAD *thrd;

MUTEXLOCK;
thrd= dcontext();
MUTEXUNLOCK;

return thrd->dbghd? thrd->dbgfp : NULL;
}

/* -----------------------------------------------------------------------
 * Modelines: {{{1
 * vim: fdm=marker ts=4
 */
