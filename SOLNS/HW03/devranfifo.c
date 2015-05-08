/* devranfifo.c: this program implements problem 3 of Homework 3
 *   Author: Charles E. Campbell
 *   Date:   Jan 20, 2014
 */

/* =====================================================================
 * Header Section: {{{1
 */

/* ---------------------------------------------------------------------
 * Includes: {{{2
 */
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "xtdio.h"

/* ------------------------------------------------------------------------
 * Definitions: {{{2
 */
#define FIFO	"devranfifo.in"

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
FILE          *fp       = NULL;
int            i;
int            qty      = 0;
int            rejected = 0;
unsigned char  uc;

Initdbg((&argc,argv));
rdcolor();

/* attempt to open the fifo (if it does not already exist) */
if(access(FIFO,R_OK) == -1) {
	if(mkfifo("devranfifo.in",0777) == -1) {
		perror("attempting to open devranfifo.in:");
		exit(-1);
		}
	}

fp= fopen(FIFO,"r");
fscanf(fp,"%d",&qty);
fclose(fp);

/* do the devran thing */
fp= fopen("/dev/random","r");
for(i= 0; i < qty; ++i) {
	do {
		uc= fgetc(fp);
		++rejected;
		} while(uc < 31 || uc >= 127);
	--rejected;
	printf("%d: %c\n",i,uc);
	}
printf("rejected %d characters\n",rejected);

Rdbg(("main 0"));
return 0;
}

/* ===================================================================== */
/* Modelines: {{{1
 *  vim: fdm=marker fenc=utf-8 ts=4
 */
