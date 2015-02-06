/* ex4.c: this program illustrates use of inline-debugging
 *        You can get dbg.h and support for Edbg Rdbg Dprintf etc from
 *        http://mysite.verizon.net/astronaut/drchipdbg/index.html
 */

/* =====================================================================
 * Header Section: {{{1
 */

/* ---------------------------------------------------------------------
 * Includes: {{{2
 */
#include <stdio.h>
#include "dbg.h"

/* ---------------------------------------------------------------------
 * Prototypes: {{{2
 */
int main( int, char **);                                   /* ex-dbg.c        */
int f1(int);                                               /* ex-dbg.c        */
int f2(int);                                               /* ex-dbg.c        */
int f3(int);                                               /* ex-dbg.c        */

/* =====================================================================
 * Functions: {{{1
 */

/* --------------------------------------------------------------------- */
/* main: {{{2 */
int main(
  int    argc,	/* count of command-line arguments    */
  char **argv)	/* array of strings from command-line */
{
int tstval;

Initdbg((&argc,argv));

/* get argument from command line */
if(argc <= 1) tstval= 0;
else          sscanf(argv[1],"%d",&tstval);

/* ok, let's see what happens... */
printf("f1(%d)=%d\n",tstval,f1(tstval));

Rdbg(("main 0"));
return 0;
}

/* --------------------------------------------------------------------- */
/* f1: this function {{{2 */
int f1(int if1)
{
int res;

Edbg(("f1(if1=%d)",if1));

++if1;
res= f2(if1) + f3(if1);

Rdbg(("f1 %d",res));
return res;
}

/* --------------------------------------------------------------------- */
/* f2: this function {{{2 */
int f2(int if2)
{
int res;

Edbg(("f2(if2=%d)",if2));

res= f3(if2+1);

Rdbg(("f2 %d",res));
return res;
}

/* --------------------------------------------------------------------- */
/* f3: this function {{{2 */
int f3(int if3)
{
int i;
int prd;

Edbg(("f3(if3=%d)",if3));

Dprintf((1,"about to go into for loop...\n"));

for(i= if3-1, prd=if3; i > 0; --i) {
	Dprintf((2,"[prd=%4d]*= [i=%2d] : %4d\n",prd,i,prd*i));
	prd*= i;
	}

Rdbg(("f3 : prd=%d",prd));
return prd;
}

/* ===================================================================== */
/* Modelines: {{{1
 *  vim: fdm=marker fenc=utf-8 ts=4
 */
