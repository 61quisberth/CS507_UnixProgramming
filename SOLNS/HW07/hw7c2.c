/* hw7c2.c: this program pays attention to stdin and sends signals to client1 (hw7c1)
 *   Author: Charles E. Campbell
 *   Date:   Apr 17, 2011
 */

/* =====================================================================
 * Header Section: {{{1
 */

/* ---------------------------------------------------------------------
 * Includes: {{{2
 */
#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#include <signal.h>
#include "xtdio.h"

/* ------------------------------------------------------------------------
 * Definitions: {{{2
 */
#define BUFSIZE 256

/* ------------------------------------------------------------------------
 * Typedefs: {{{2
 */

/* ------------------------------------------------------------------------
 * Local Data Structures: {{{2
 */

/* ------------------------------------------------------------------------
 * Global Data: {{{2
 */

/* ------------------------------------------------------------------------
 * Explanation: {{{2
 */

/* ------------------------------------------------------------------------
 * Prototypes: {{{2
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
char buf[BUFSIZE];
int  hw7c1;
int  isig;
int  qty;
int  sig0;
int  sign;

Initdbg((&argc,argv));
rdcolor();

if(!argv[1]) {
	fprintf(stderr,"***error*** (hw7c2) argv[1] should hold pid string but its null!\n");
	exit(-1);
	}
sscanf(argv[1],"%d",&hw7c1);
printf("hw7c2 pid#%d\n",getpid());

printf("Enter signal: ");
while(fgets(buf,BUFSIZE,stdin)) {
	buf[strlen(buf)-1]= '\0'; /* remove trailing newline */
	Dprintf((2,"buf<%s>\n",sprt(buf)));
	if(isdigit(buf[0])) {
		qty= sscanf(buf,"%d-%d",&sig0,&sign);
		Dprintf((2,"sig0=%d sign=%d\n",sig0,sign));
		if(qty == 1) sign= sig0;
		else if(sig0 > sign) { /* swap */
			isig = sig0;
			sig0 = sign;
			sign = isig;
			}
		Dprintf((1,"will send signals#%d-%d to pid#%d\n",sig0,sign,hw7c1));
		for(isig= sig0; isig <= sign; ++isig) {
			Dprintf((1,"kill(hw7c1#%d,sig#%2d)\n",hw7c1,isig));
			kill(hw7c1,isig);
			}
		}
	else if(!strcmp(buf,"q")) break;
	printf("Enter signal: ");
	}

Rdbg(("main 0 : hw7c2 terminating"));
return 0;
}

/* ===================================================================== */
/* Modelines: {{{1
 *  vim: fdm=marker fenc=utf-8 ts=4
 */
