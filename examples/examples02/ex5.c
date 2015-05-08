/* ex5.c: this program illustrates string manipulation
 *   Author: Charles E. Campbell, Jr.
 *   Date:   Sep 09, 2010
 */

/* =====================================================================
 * Header Section: {{{1
 */

/* ---------------------------------------------------------------------
 * Includes: {{{2
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ========================================================================
 * Functions: {{{1
 */

/* --------------------------------------------------------------------- */
/* main: {{{2 */
int main(
  int    argc,
  char **argv)
{
char  inpbuf[256];
char *s1          = NULL;
char *s2          = NULL;
char *s3          = NULL;

/* get a string from the user into a fixed-size buffer
 * allocate a new buffer using calloc that's just big enough to hold that string
 * copy input buffer string into newly allocated buffer
 *
 * Repeat for a second string
 */
printf("Enter string1: ");
scanf("%s",inpbuf);
s1= (char *) calloc((size_t) strlen(inpbuf)+1,sizeof(char));
strcpy(s1,inpbuf);
printf("Got s1<%s>\n",s1);

printf("Enter string2: ");
scanf("%s",inpbuf);
s2= (char *) calloc((size_t) strlen(inpbuf)+1,sizeof(char));
strcpy(s2,inpbuf);
printf("Got s2<%s>\n",s2);

/* concatenate the two strings with an intervening space */
s3= (char *) calloc((size_t) strlen(s1) + strlen(s2) + 2,sizeof(char));

sprintf(s3,"%s %s",s1,s2);
printf("Method 1: s3<%s>  (used sprintf)\n",s3);

strcpy(s3,s1);
s3[strlen(s1)]= ' ';
strcpy(s3+strlen(s1)+1,s2);
printf("Method 2: s3<%s>  (used strcpy)\n",s3);

return 0;
}

/* ===================================================================== */
/* Modelines: {{{1
 *  vim: fdm=marker fenc=utf-8 ts=4
 */
