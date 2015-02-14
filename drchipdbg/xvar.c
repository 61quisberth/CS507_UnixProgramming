/* xvar.c: this file contains settings, flags, etc for the xtdio library	*/
#include <stdio.h>
#include "xtdio.h"

#if defined(MSDOS) || defined(__BORLANDC__)
# define msdos
#endif

/* ---------------------------------------------------------------------
 * Rdcolor support:
 */
int   screenheight      = 25;		/* rows    on the screen						*/
int   screenwidth       = 80;		/* columns in a line available to matout		*/
int   use_escapes       = 1;		/* (don't) use ANSI escape sequences			*/

/* ---------------------------------------------------------------------
* Outofmem and Error Support
*/
#ifdef __PROTOTYPE__
void exit(int);
void (*error_exit)(int) = exit;
#else
void (*error_exit)()    = exit;
#endif
FILE *errfp             = NULL;

/* ---------------------------------------------------------------------
* Rdcolor support
*/
char *BLACK             = "";
char *RED               = "";
char *GREEN             = "";
char *YELLOW            = "";
char *BLUE              = "";
char *MAGENTA           = "";
char *CYAN              = "";
char *WHITE             = "";

/* since some systems just have to use BLACK, ..., WHITE for themselves... */
char *RD_BLACK          = "";
char *RD_RED            = "";
char *RD_GREEN          = "";
char *RD_YELLOW         = "";
char *RD_BLUE           = "";
char *RD_MAGENTA        = "";
char *RD_CYAN           = "";
char *RD_WHITE          = "";
char *RD_CLEAR          = "";

char *UBLACK            = "";
char *URED              = "";
char *UGREEN            = "";
char *UYELLOW           = "";
char *UBLUE             = "";
char *UMAGENTA          = "";
char *UCYAN             = "";
char *UWHITE            = "";

char *RVBLACK           = "";
char *RVRED             = "";
char *RVGREEN           = "";
char *RVYELLOW          = "";
char *RVBLUE            = "";
char *RVMAGENTA         = "";
char *RVCYAN            = "";
char *RVWHITE           = "";

char *ITBLACK           = "";
char *ITRED             = "";
char *ITGREEN           = "";
char *ITYELLOW          = "";
char *ITBLUE            = "";
char *ITMAGENTA         = "";
char *ITCYAN            = "";
char *ITWHITE           = "";

char *BOLDBLACK         = "";
char *BOLDRED           = "";
char *BOLDGREEN         = "";
char *BOLDYELLOW        = "";
char *BOLDBLUE          = "";
char *BOLDMAGENTA       = "";
char *BOLDCYAN          = "";
char *BOLDWHITE         = "";

char *CLEAR             = "";
char *BOLD              = "";
char *NRML              = "";

char *PYELLOW           = "";
char *PRED              = "";
char *PCYAN             = "";
char *PBLACK            = "";

/* --------------------------------------------------------------------- */
