/* strprintf.c: this function acts like a member of the printf family,
 *  except that it produces its output onto an internal buffer
 *  and returns a pointer to it.
 */

/* =====================================================================
 * Header Section: {{{1
 */

/* ---------------------------------------------------------------------
 * Includes: {{{2
 */
#include <stdio.h>
#include "xtdio.h"

/* ----------------------------------------------------------------------
 * Definitions: {{{2
 */
#define BUFSIZE   256
#define BUFMARGIN 64

/* =====================================================================
 * Functions: {{{1
 */

/* ---------------------------------------------------------------------- */
/* strprintf: printf-style formatted printing to internal buffer {{{2 */
char *strprintf(char *fmt,...)
{
va_list         args;
size_t          flen;
static char    *buf1  = NULL;
static char    *buf2  = NULL;
static char    *buf3  = NULL;
static char    *buf4  = NULL;
static char    *buf5  = NULL;
static char    *buf6  = NULL;
static char    *buf7  = NULL;
static char    *buf8  = NULL;
static char    *buf9  = NULL;
static char    *buf10 = NULL;
static char   **buf   = &buf1;
static size_t  *len   = NULL;
static size_t   len1  = 0;
static size_t   len2  = 0;
static size_t   len3  = 0;
static size_t   len4  = 0;
static size_t   len5  = 0;
static size_t   len6  = 0;
static size_t   len7  = 0;
static size_t   len8  = 0;
static size_t   len9  = 0;
static size_t   len10 = 0;

/* return old buffer when format is null pointer */
if(!fmt) return *buf;

/* get new buffer */
if     (*buf == buf1) buf= &buf2 , len= &len2 ;
else if(*buf == buf2) buf= &buf3 , len= &len3 ;
else if(*buf == buf3) buf= &buf4 , len= &len4 ;
else if(*buf == buf4) buf= &buf5 , len= &len5 ;
else if(*buf == buf5) buf= &buf6 , len= &len6 ;
else if(*buf == buf6) buf= &buf7 , len= &len7 ;
else if(*buf == buf7) buf= &buf8 , len= &len8 ;
else if(*buf == buf8) buf= &buf9 , len= &len9 ;
else if(*buf == buf9) buf= &buf10, len= &len10;
else                  buf= &buf1 , len= &len1;

/* automatic buffer (re-)allocation */
flen= strlen(fmt);
if(flen + BUFSIZE > *len) {
	*len= flen + BUFSIZE + BUFMARGIN;
	if(!*buf) *buf= (char *) malloc(*len);
	else      *buf= realloc(*buf,*len);
	}

va_start(args,fmt);
vsnprintf(*buf,(size_t)BUFSIZE,fmt,args);
va_end(args);

return *buf;
}

/* ===================================================================== */
/* Modelines: {{{1
 *  vim: fdm=marker
 */
