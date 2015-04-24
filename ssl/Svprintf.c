/* Svprintf.c:
 * Authors:      Charles E. Campbell, Jr.
 *               Terry McRoberts
 * Copyright:    Copyright (C) 1999-2010 Charles E. Campbell, Jr.
 *               Permission is hereby granted to use and distribute this code,
 *               with or without modifications, provided that this copyright
 *               notice is copied with it. Like anything else that's free,
 *               Saccept.c is provided *as is* and comes with no warranty
 *               of any kind, either expressed or implied. By using this
 *               software, you agree that in no event will the copyright
 *               holder be liable for any damages resulting from the use
 *               of this software.  There is no merchantability or fitness
 *               for a particular purpose.
 * Date:         Nov 18, 2010
 */
#include <stdio.h>
#include "sockets.h"

/* --------------------------------------------------------------------- */

/* Svprintf:
 *
 *   Returns: count of bytes sent, which may be 0
 *
 *   This function is like vprintf; it takes a Socket pointer, a format
 *   string, and an argument list.  Note that it actually "prints" the
 *   string into a local buffer first of PM_BIGBUF bytes (originally 1024
 *   bytes).  So, please insure that you don't put more info than will fit
 *   into PM_BIGBUF bytes!  It then squirts the resulting string through
 *   a call to Swrite.
 */
#ifdef __PROTOTYPE__
int Svprintf(
  Socket *skt,
  char   *fmt,
  void   *args)
#else
int Svprintf(skt,fmt,args)
Socket *skt;
char   *fmt;
void   *args;
#endif
{
int         ret;
static char buf[PM_BIGBUF];


/* sanity check */
if(!skt) {
	return 0;
	}

#ifdef AS400
ret= vsprintf(buf,fmt,__va_list args);
#else
ret= vsprintf(buf,fmt,(void *) args);
#endif
Swrite(skt,buf,strlen(buf)+1);	/* send the null byte, too */

return ret;
}

/* ---------------------------------------------------------------------
 * vim: ts=4
 */
