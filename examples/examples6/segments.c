/* segments.c: this program illustrates segment addresses under Linux
 *   Author: Charles E. Campbell
 *   Date:   Mar 06, 2014
 */

/* =====================================================================
 * Header Section: {{{1
 */

/* ---------------------------------------------------------------------
 * Includes: {{{2
 */
#include <stdio.h>
#include "xtdio.h"

/* ------------------------------------------------------------------------
 * External Data: {{{2
 */
extern int  etext;
extern int  edata;
extern int  end;

/* ------------------------------------------------------------------------
 * Prototypes: {{{2
 */
int main( int, char **);       /* segments.c */
char *IdentifySegment(void *); /* segments.c */

/* ------------------------------------------------------------------------
 * Explanation: {{{2
 */

/* ========================================================================
 * Functions: {{{1
 */

/* --------------------------------------------------------------------- */
/* main: {{{2 */
int main(
  int    argc,
  char **argv)
{
char       *s1    = "hello"; /* initialized data segment   */
static int  a     = 1;       /* initialized data segment   */
static int  b;               /* uninitialized data segment */
char        s2[]  = "hello"; /* stack area                 */
int        *c;

Initdbg((&argc,argv));
rdcolor();

c= malloc(sizeof(int));      /* in the stack area          */
printf("Address of etext: %p (first valid address above the text)\n",&etext);
printf("Address of edata: %p (first valid address above the initialized data segment)\n",&edata);
printf("Address of end  : %p (first valid address above the uninitialized data segment)\n",&end);

printf("in   initialized data segment: char      *s1=\"hello\":    s1=%px  %s\n",s1,IdentifySegment(s1));
printf("in   initialized data segment: int         a=1:          &a=%px  %s\n",&a,IdentifySegment(&a));
printf("in uninitialized data segment: static int  b:            &b=%px  %s\n",&b,IdentifySegment(&b));
printf("in         stack data segment: char       s2[]=\"hello\":  s2=%px  %s\n",s2,IdentifySegment(s2));
printf("in          heap data segment: int        *c:             c=%px  %s\n",&c,IdentifySegment(c));

Rdbg(("main 0"));
return 0;
}

/* --------------------------------------------------------------------- */
/* IdentifySegment: this function {{{2 */
char *IdentifySegment(void *p)
{
if(p > (void *) &end)   return "above uninitialized data (ie. stack, heap)";
if(p > (void *) &edata) return "above initialized data (ie. bss)";
if(p > (void *) &etext) return "above text";
return "?";
}

/* ===================================================================== */
/* Modelines: {{{1
 *  vim: fdm=marker fenc=utf-8 ts=4
 */
