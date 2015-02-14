#include <stdio.h>
#include <ctype.h>

/* --------------------------------------------------------------------- */
  
/* stpblk: skips blanks (white space) */
char *stpblk(char *p) 
{
while(isspace(*p)) ++p;
return p;
}

/* --------------------------------------------------------------------- */
