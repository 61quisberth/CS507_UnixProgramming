/* uplister.c: this program implements hw04: which lists directories going up the directory chain
 *   Author: Charles E. Campbell
 *   Date:   Mar 05, 2011
 */

/* =====================================================================
 * Header Section: {{{1
 */

/* ---------------------------------------------------------------------
 * Includes: {{{2
 */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>
#include <limits.h>
#include "xtdio.h"

/* ------------------------------------------------------------------------
 * Definitions: {{{2
 */

/* ------------------------------------------------------------------------
 * Typedefs: {{{2
 */

/* ------------------------------------------------------------------------
 * Local Data Structures: {{{2
 */

/* ------------------------------------------------------------------------
 * Global Data: {{{2
 */
static char path[PATH_MAX+1];

/* ------------------------------------------------------------------------
 * Explanation: {{{2
 */

/* ------------------------------------------------------------------------
 * Prototypes: {{{2
 */
int main( int, char **);    /* uplister.c */
void UpLister(char *, int); /* uplister.c */

/* ========================================================================
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

if(!getcwd(path,PATH_MAX+1)) {
	perror("unable to getcwd current directory");
	exit(-1);
	}
UpLister(path,0);

Rdbg(("main 0"));
return 0;
}

/* --------------------------------------------------------------------- */
/* UpLister: this function lists directory contents and uses recursion to go *up* a directory {{{2 */
void UpLister(char *dirname, int depth)
{
DIR           *dir;
struct dirent *file;
int            i;
char          *slash;

Edbg(("UpLister(dirname<%s>,depth=%d)",sprt(dirname),depth));

dir= opendir(dirname);
if(!dir) {
	perror(dirname);
	exit(-1);
	}

/* give current directory name */
slash= strrchr(path,'/');
if(slash) { /* show directory name */
	for(i= 0; i < depth-1; ++i) printf("  ");
	printf("%s:\n",slash+1);
	}
else {      /* show root, too      */
	for(i= 0; i < depth-1; ++i) printf("  ");
	printf("/:\n");
	}

while((file= readdir(dir)) != NULL) {
	Dprintf((1,"file<%s> type=%s\n",
	  sprt(file->d_name),
	  (file->d_type == DT_DIR)?     "dir"     :
	  (file->d_type == DT_FIFO)?    "fifo"    :
	  (file->d_type == DT_LNK)?     "lnk"     :
	  (file->d_type == DT_REG)?     "reg"     :
	  (file->d_type == DT_BLK)?     "blk"     :
	  (file->d_type == DT_CHR)?     "chr"     :
	  (file->d_type == DT_SOCK)?    "sock"    :
	  (file->d_type == DT_UNKNOWN)? "unknown" : "???"));
	if(!strcmp(file->d_name,"."))  continue;
	if(!strcmp(file->d_name,"..")) continue;

	for(i= 0; i < depth; ++i) printf("  ");
	printf("%s%s\n",
	  file->d_name,
	  (file->d_type == DT_DIR)?  "/" :
	  (file->d_type == DT_FIFO)? "|" :
	  (file->d_type == DT_LNK)?  "@" :
	  (file->d_type == DT_REG)?  ""  : "?");
	}
if((slash= strrchr(path,'/'))) {
	*slash= '\0'; /* strip off current directory's name */
	if(path[0] == '\0') UpLister("/",depth+1);
	else                UpLister(path,depth+1);
	}

Rdbg(("UpLister"));
}

/* ===================================================================== */
/* Modelines: {{{1
 *  vim: fdm=marker fenc=utf-8 ts=4
 */
