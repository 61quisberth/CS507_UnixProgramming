/* vacuum.c: this program illustrates the vacuum cleaner in C
 * In my links.h header file, included by "xtdio.h", is the following
 * macro: (expanded by the preprocessor)
 *
 * #define stralloc(ptr,string,fail_msg) {                          \
 * 	ptr= (char *) calloc((size_t) strlen(string) + 1,sizeof(char)); \
 * 	outofmem(ptr,"%s: <%s>\n",fail_msg,string);                     \
 * 	strcpy(ptr,string);                                             \
 * 	}
 */
#include <stdio.h>
#include <unistd.h>
#include "xtdio.h"

/* ------------------------------------------------------------------------
 * Definitions: {{{1
 */
#define VACLEN	4
#define BUFSIZE	256
#define ROWS	25
#define COLS	80-VACLEN

/* ------------------------------------------------------------------------
 * Typedefs: {{{1
 */

/* ------------------------------------------------------------------------
 * Local Data Structures: {{{1
 */

/* ------------------------------------------------------------------------
 * Global Data: {{{1
 */

/* ------------------------------------------------------------------------
 * Explanation: {{{1
 */

/* ------------------------------------------------------------------------
 * Prototypes: {{{1
 */

/* ------------------------------------------------------------------------
 * Source Code: {{{1
 */

/* main: {{{2 */
int main(
  int    argc,
  char **argv)
{
int          row;
int          rows          = ROWS;
int          col;
int          cols          = COLS;
static char *goright[COLS+VACLEN];        /* an array of pointers to strings             */
static char *goleft[COLS+VACLEN];         /* an array of pointers to strings             */
int          vaclen;

Initdbg((&argc,argv));
rdcolor();

if(argc >= 3) { /* allow user to type vacuum rows cols */
	sscanf(argv[1],"%d",&rows);
	sscanf(argv[1],"%d",&cols);
	cols-= 4;
	if(rows < 1) rows= ROWS;
	if(cols < 1) cols= COLS;
	}

/* build goright and goleft */
Dprintf((1,"build goright and goleft\n"));
stralloc(goright[0]    , "\\==<", "Initial vacuum cleaner");
stralloc(goleft[cols-1], ">==/" , "Initial vacuum cleaner");
vaclen= strlen(goright[0]);
for(col= 1; col < cols; ++col) {
	++vaclen;
	goright[col]       = (char *) calloc((size_t) vaclen+1,sizeof(char));
	goleft[cols-1-col] = (char *) calloc((size_t) vaclen+1,sizeof(char));
	sprintf(goright[col]," %s",goright[col-1]);
	sprintf(goleft[cols-1-col]," %s",goleft[cols-col]);
	Dprintf((1,"goright[%2d][%s]\n",col       ,sprt(goright[col])));
	Dprintf((1,"goleft [%2d][%s]\n",cols-1-col,sprt(goleft[cols-1-col])));
	}

/* draw the vacuum cleaner */
Dprintf((1,"\ndraw the vacuum cleaner: rows=%d cols=%d\n",rows,cols));
for(row= 0; row < rows; ++row) {
	if(row > 0) printf("\033[%d;1H\033[K",row);
	printf("\033[%d;1H",row+1);
	if(row % 2) {
		/* goleft */
		for(col= 0; col < cols; ++col) {
			printf("\033[%d;1H",row+1);
			printf("%s\033[K",goleft[col]);
			fflush(stdout);
			usleep(5000L);	/* 1ms == 1000 , so 5000 -> 5ms */
			}
		}
	else {
		/* goright */
		for(col= 0; col < cols; ++col) {
			printf("\033[%d;1H",row+1);
			printf("%s",goright[col]);
			fflush(stdout);
			usleep(5000L);	/* 1ms == 1000 , so 5000 -> 5ms */
			}
		}
	printf("\033[%d;1H\033[K",row);
	}

/* emit the loud BURP! */
printf("\033[%d;1H\033[K\\==< o*8o BURP! o.8o\n",row);

Rdbg(("main 0"));
return 0;
}

/* ---------------------------------------------------------------------
 * vim: fdm=marker
 */
