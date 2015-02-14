/* dbg.h: Debugging Routines header file
 *
 *	The debugging system is energized by calling
 *
 *		initdbg(&argc,argv);
 *
 *	early in the main program.  initdbg() looks for a
 *
 *		-@ dbgfile [dbgfile2 ...]  [# [#]]
 *  or
 *		-@ "{on|ondis|keep|off} funcname [*] [detail-level] [:dtst1 [:dtst2]] [preq list]"
 *
 *	and processes a dbgfile in the first case.  The two optional numbers are
 *	also read into the variables dtst1 and dtst2.  If the optional numbers are
 *	not present on the command line, then both will be set to DBG_DTSTOFF, which
 *	currently is -2.  Note: debug files may not begin with an integer or minus sign.
 *
 *	The second case is detected by having a string begin with "on" or "of";
 *	in that case, the argument is passed directly to initdbg3().
 *
 * Debugging Commands: {{{1
 *	The debugging system exhibits function-level control over printing of
 *	debugging information.  The dbgmode contains three pieces of information:
 *	a detail level (between 0 to 15), whether or not debugging is currently
 *	active (ie. on), and whether debugging is to be inherited by deeper
 *	functions.  Debugging output is preceded by a number of dots indicating
 *	the current debugging function depth.
 *
 *	The debug file contains lines following one of the following forms:
 *
 *    {on|ondis|off} funcname [*] [detail-level] [:dtst1 [:dtst2]] [preq list] [>file]
 *    keep funcname [:dtst1 [:dtst2]]
 *    del [funcname | *]
 *
 *	on|off      : during program flow, debugging will be turned on/off
 *	              if the associated function name (funcname) is encountered.
 *  keep        : don't modify debugging status, but allow changes to dtst1,2
 *  ondis       : debugging for the given function is unchanged, but
 *                dbgswitch() can be used to turn it on.
 *	funcname    : debugging information concerns this function name
 *	*           : debugging mode will not be inherited by deeper
 *	              routines (default: debug mode will be inherited)
 *	detail-level: debugging detail levels are in the interval [0,15],
 *	              with 0 as default (and least detail).
 *  :dtst1      : a colon-number signifies a dtst1 value
 *  :dtst2      : a colon-number signifies a dtst2 value
 *	preq-list   : a list of function names, optionally preceded with no
 *	              intervening white space by a "!".  These functions must
 *	              be present in the current routine stack (or not present
 *	              if preceded by a "!") for the debugging command to be fire.
 *  >filename   : debugging output will be directed to this file.  It
 *                will be opened once, and debugging output appended to,
 *                the given file.
 *
 *	del funcname: delete debugging info on funcname from debugging system
 *	del *       : delete all debugging info from debugging system
 *
 *  Routines Include: from <dbg.c> {{{1
 *
 *    void prt_traceback()
 *         prints the current routine stack (quaintly known as a traceback)
 *
 *    void edbg(char *fmt,...)
 *         edbg("funcname(...)",...): the function appears like a member of
 *         the printf() family, except that the string must begin with
 *         a funcname.  The funcname is checked against the debug file info
 *         and, if the funcname appears in the debugging database, the
 *         dbgmode is modified appropriately.  This call is to be placed at
 *         the beginning of every function (to be debugged); it signifies
 *         that the function has been entered. The string will be printed if
 *         the dbgmode so indicates at a detail-level of zero.
 *
 *    void rdbg(char *fmt,...)
 *         rdbg("funcname ...",...): the function appears like a member of
 *         the printf() family, except that the format string must begin with
 *         the current funcname.  This call is to be placed just before every
 *         return point; it signifies that the function is returning.  The
 *         string will be printed if the dbgmode so indicates at a
 *         detail-level of zero.
 *
 *    int  dprintf(int detail,char *fmt,...)
 *         dprintf(detail-level,"...",...): except for the first integer
 *         argument, this function appears like a member of the printf()
 *         family.  The detail-level is an integer between 0 to 15.  The data
 *         will be printed with the usual leading dots indicating routine
 *         depth.  This function returns a 1 if debugging is enabled, 0 else.
 *
 *    int  dbgswitch(int on)
 *         If dtst1 and optionally dtst2 were entered on the command line,
 *         then dbgswitch can be used to further delineate debugging
 *         selection.  Basically, dbgswitch takes a true/false (0=false,
 *         other=true) and will turn dbgmode on/off as indicated.  dtst1 and
 *         dtst2 may, of course, be used in the test.
 *
 *    void initdbg(int *argc,char **argv)
 *         Call initdbg(&argc,argv) at the beginning of the main program.
 *
 *    void initdbg2(char *dbgfile)
 *         The function processes all the lines in the dbgfile using
 *         initdbg3.  It is called by initdbg().
 *
 *    void initdbg3(char *buf)
 *         The function processes a single line, usually provided by
 *         initdbg2(), and modifies the debugging database.
 *
 *    void prt_dbgstr()
 *         Prints out the entire debugging database.  This function is useful
 *         for documenting a particular debugging run with what debugging
 *         information was used.
 *
 *    int dbgtst(int detail)
 *         This function tests to see if a dprintf() at the specified detail
 *         level would print or not.
 *
 */
#ifndef DBG_H
#define DBG_H
/*#define DRCDBGLINE	|+ enables ~LINENUMBERs for locating where a debugging line came from +|*/

#if !defined(XTDIO_H) && defined(__cplusplus)
extern "C" {
#endif

/* ---------------------------------------------------------------------
 * Includes: {{{1
 */
#include <stdio.h>

#ifndef SETPROTO_H
# include "setproto.h"
#endif
#ifdef dprintf
# undef dprintf
#endif
#if defined(__linux) || defined(sgi)
# define dprintf	dprintf_linux
#elif defined(__CYGWIN__)
# define dprintf	dprintf_cygwin
#endif

/* Debugging Macros: {{{1 */
#if defined(DEBUG) || defined(CECDEBUG)	/* #ifdef DEBUG ... #else ... #endif	*/

/* Convenient functions to use without #ifdef DEBUG ... #endif blocks
 * Use with *two* parentheses, otherwise you'll have syntax problems:
 * Until C allows variable argument lists in macros, this is the best
 * that can be done:
 *
 *   ex. Edbg(("usual()"));
 *   ex. if(Dbgtst((1))) { ... }
 *   ex. Prt_traceback(());
 *
 */

#define Dbgfpget(x)			dbgfpget x
#define Dbgswitch(x)		dbgswitch x
#define Dbgchkswitch(x)		dbgchkswitch x
#define Dbgtst(x)			dbgtst x
#define Dlongjmp(x)			dlongjmp x
#define Dprintf(x)			DFL dprintf x
#define Dsetjmp(x)			dsetjmp x
#define Edbg(x)				edbg x
#define ElseDprintf(x)		else DFL dprintf x
#define IfDprintf(tst,x)	if tst DFL dprintf x
#define Initdbg(x)			initdbg x
#define Initdbg2(x)			initdbg2 x
#define Initdbg3(x)			initdbg3 x
#define Prt_dbgstr(x)		prt_dbgstr x
#define Prt_traceback(x)	prt_traceback x
#define Rdbg(x)				DFL rdbg x
#define DDmatout(lvl,x)		if(dbgtst(lvl)) dbgDmatout x
#define DDvecout(lvl,x)		if(dbgtst(lvl)) dbgDvecout x
#define DCmatout(lvl,x)		if(dbgtst(lvl)) dbgCmatout x
#define DCvecout(lvl,x)		if(dbgtst(lvl)) dbgCvecout x
#if defined(__STDC__) && defined(DRCDBGLINE)
# define DFL					drcdbgfile=__FILE__,drcdbgline=__LINE__,
#else
# define DFL
#endif

#if defined(__GNUC__) || defined(__GNUG__)
# define Edbgf				edbg("%s()",__FUNCTION__);
# define Rdbgf				rdbg("%s",__FUNCTION__);
#elif defined(STDC99)
# define Edbgf				edbg("%s()",__func__);
# define Rdbgf				rdbg("%s",__func__);
#else
# define Edbgf
# define Rdbgf
#endif

/* -------------------------------------------------------------------------
 * Debug Definitions: {{{1
 */
#define DBG_DTSTOFF		-10000		/* indicates a dtst "off" condition			*/
#define ALLOCTESTSIZE	20

#define alloctest { \
  char *junk; \
  junk= (char *) calloc((size_t) ALLOCTESTSIZE,sizeof(char)); \
  strcpy(junk,"junk"); \
  free(junk); \
  }

/* -------------------------------------------------------------------------
 * Typedefs: {{{1
 */

/* -------------------------------------------------------------------------
 * Debugging Variables: {{{1
 */
#ifdef DBG_MAIN

char  dbgnestchar = '|';         /* character printed for nesting            */
int   dtst1       = DBG_DTSTOFF; /* useful for dbgswitch()                   */
int   dtst2       = DBG_DTSTOFF; /* useful for dbgswitch()                   */
int   dbglvl      = 0;           /* current NESTing level                    */
int   dbg_Nflag   = 0;           /* user may have dprintf: ....func: if true */
int   idbg        = 0;           /* used by NEST                             */
char *drcdbgfile  = NULL;        /* used by dprintf                          */
int   drcdbgline  = 0;           /* used by dprintf                          */

#else

extern char  dbgnestchar;
extern int   dtst1;
extern int   dtst2;
extern int   dbglvl;
extern int   dbg_Nflag;
extern int   idbg;
extern char *drcdbgfile;
extern int   drcdbgline;

#endif	/* #ifdef DBG_MAIN #else ... #endif */

/* Null Debugging Macros: {{{1 */
#else	/* #ifdef DEBUG ... #else ... #endif	*/

#define Dbgfpget(x)		stdout
#define Dbgswitch(x)
#define Dbgchkswitch(x)
#define Dbgtst(x)		0
#define Dlongjmp(x)
#define Dprintf(x)
#define Dsetjmp(x)
#define Edbg(x)
#define ElseDprintf(x)
#define IfDprintf(tst,x)
#define Initdbg(x)
#define Initdbg2(x)
#define Initdbg3(x)
#define Prt_dbgstr(x)
#define Prt_traceback(x)
#define Rdbg(x)

# define Edbgf
# define Rdbgf
# define DDmatout(lvl,x)
# define DDvecout(lvl,x)
# define DCmatout(lvl,x)
# define DCvecout(lvl,x)

#endif	/* #ifdef DEBUG ... #else ... #endif	*/

#if !defined(XTDIO_H) && defined(__cplusplus)
	}
#endif

#endif	/* #ifdef DBG_H ... #endif				*/

/* --------------------------------------------------------------------- */
/* Modelines: {{{1
 * vim: fdm=marker
 */
