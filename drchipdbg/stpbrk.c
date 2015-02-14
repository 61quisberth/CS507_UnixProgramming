/* stpbrk: this function returns a pointer to a break
 *  character in s, if there is one.
 */
#include <stdio.h>

/* --------------------------------------------------------------------- */

char *stpbrk(char *s,char *brk)
{
char *b;

for(; *s; ++s) {
	for(b= brk; *b; ++b) if(*s == *b) break;
	if(*b) return s;
	}
return (char *) NULL;
}

/* --------------------------------------------------------------------- */
