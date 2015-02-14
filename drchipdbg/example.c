/* example.c: this program illustrates how to use DrChip's inline debugger
 */

/* =====================================================================
 * Header Section: {{{1
 */

/* ---------------------------------------------------------------------
 * Includes: {{{2
 */
#include <stdio.h>
#include "dbg.h"

/* ------------------------------------------------------------------------
 * Prototypes: {{{2
 */
int main( int, char **); /* example.c */
void f1(int);            /* example.c */
void f2(void);           /* example.c */
int f3(int);             /* example.c */

/* =====================================================================
 * Functions: {{{1
 */

/* --------------------------------------------------------------------- */
/* main: {{{2 */
int main(
  int    argc,
  char **argv)
{
Initdbg((&argc,argv));
rdcolor();

f1(0);
f2();

Rdbg(("main 0"));
return 0;
}

/* --------------------------------------------------------------------- */
/* f1: this function {{{2 */
void f1(int i)
{
Edbg(("f1(i=%d)",i));

if(i == 0) f2();
else       f3(i);

Rdbg(("f1"));
}

/* --------------------------------------------------------------------- */
/* f2: this function {{{2 */
void f2(void)
{
Edbg(("f2()"));
f3(3);
Rdbg(("f2"));
}

/* --------------------------------------------------------------------- */
/* f3: this function {{{2 */
int f3(int j)
{
int res;

Edbg(("f3(j=%d)",j));
for(res= 1; j; --j) {
	res*= j;
	Dprintf((2,"res*= [j=%d] =%d\n",j,res));
	}

Rdbg(("f3 %d",res));
return res;
}

/* ===================================================================== */
/* Modelines: {{{1
 *  vim: fdm=marker
 */
