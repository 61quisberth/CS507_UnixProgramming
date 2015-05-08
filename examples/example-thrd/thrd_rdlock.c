/* thrd_rdlock.c: this program exercises reader/writer locks
 *   Author: Charles E. Campbell
 *   Date:   Apr 19, 2013
 */

/* =====================================================================
 * Header Section: {{{1
 */

/* ---------------------------------------------------------------------
 * Includes: {{{2
 */
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include "xtdio.h"
#include "xmath.h"

/* ------------------------------------------------------------------------
 * Definitions: {{{2
 */
#define BUFSIZE   256

/* ------------------------------------------------------------------------
 * Global Data: {{{2
 */
int                   fd;
int                   rctr       = 0;
int                   thrdctr    = 0;
int                   thrdmax    = 10;
int                   wctr       = 0;
off_t                 lastwrite  = -1;
long                  s1;
long                  s2;
long                  s3;
pthread_rwlockattr_t  rwlockattr;
pthread_rwlock_t      rwlock;
pthread_t            *thrd       = NULL;

/* ------------------------------------------------------------------------
 * Prototypes: {{{2
 */
int main( int, char **); /* thrd_rdlock.c */
void thrdReader(int *);  /* thrd_rdlock.c */
void thrdWriter(int *);  /* thrd_rdlock.c */

/* ========================================================================
 * Functions: {{{1
 */

/* --------------------------------------------------------------------- */
/* main: {{{2 */
int main(
  int    argc,
  char **argv)
{
int       i;
double    r;

Initdbg((&argc,argv));
rdcolor();

/* read in optional qty of threads from command line */
if(argc > 1) sscanf(argv[1],"%d",&thrdmax);

/* allocate an array of threads */
thrd= (pthread_t *) calloc((size_t) thrdmax,sizeof(pthread_t));

init_rndm(&s1,&s2,&s3);
printf("main   initialize a rwlock\n");
pthread_rwlockattr_setpshared(&rwlockattr,PTHREAD_PROCESS_PRIVATE);
pthread_rwlock_init(&rwlock,&rwlockattr);

printf("main   opening file<thrd_rdlock.dat> for reading and writing\n");
fd= open("thrd_rdlock.dat",O_CREAT|O_TRUNC|O_RDWR,S_IRWXU);
if(fd < 0) {
	perror("main : attempted to open<thrd_rdlock.dat>");
	exit(1);
	}
printf("main   file descriptor#%2d\n",fd);

/* create and run threads */
for(i= 0; i < thrdmax; ++i) {
	r= rndm();
	if     (i == 0) pthread_create(&thrd[i],NULL,(void *) thrdWriter,&fd); /* guaranteed to have one writer                 */
	else if(i == 1) pthread_create(&thrd[i],NULL,(void *) thrdReader,&fd); /* and guaranteed to have one reader (thrdmax>1) */
	else if(r < .5) pthread_create(&thrd[i],NULL,(void *) thrdReader,&fd);
	else            pthread_create(&thrd[i],NULL,(void *) thrdWriter,&fd);
	}

/* join all threads */
for(i= 0; i < thrdmax; ++i) {
	printf("main   : waiting for thread#%-2d to join\n",i);
	fflush(stdout);
	pthread_join(thrd[i],(void **) NULL);
	}

/* destroy r/w lock
 * close file descriptor
 */
pthread_rwlock_destroy(&rwlock);
close(fd);

Rdbg(("main 0"));
return 0;
}

/* --------------------------------------------------------------------- */
/* thrdReader: this function reads a message from a common resource {{{2 */
void thrdReader(int *pfd)
{
char  buf[BUFSIZE];
float delay;
int   thrdid;

Edbg(("thrdReader(%spfd)",pfd? "" : "null "));

/* figure out the "thrdid" */
for(thrdid= 0; thrdid < thrdmax; ++thrdid) if(pthread_self() == thrd[thrdid]) break;

printf("reader thread#%-2d: (%2d readers, %2d writers) attempting to acquire rwlock\n",thrdid,rctr,wctr);
pthread_rwlock_rdlock(&rwlock);
++rctr;
if(lastwrite >= 0) lseek(*pfd,lastwrite,SEEK_SET);
read(*pfd,buf,BUFSIZE);
printf("reader thread#%-2d: (%2d readers, %2d writers) read message<%s> from lastwrite=%ld\n",thrdid,rctr,wctr,sprt(buf),lastwrite);
delay= rndm()*3000000.;
usleep((useconds_t) delay); /* wait some amount of time between [0,3] seconds */
printf("reader thread#%-2d: (%2d readers, %2d writers) releasing rwlock after %9.6fsec delay\n",thrdid,rctr,wctr,delay/1000000.);
pthread_rwlock_unlock(&rwlock);
--rctr;
printf("reader thread#%-2d: (%2d readers, %2d writers) rwlock released\n",thrdid,rctr,wctr);

Rdbg(("thrdReader"));
}

/* --------------------------------------------------------------------- */
/* thrdWriter: this function writes a message to a common resource {{{2 */
void thrdWriter(int *pfd)
{
char       buf[BUFSIZE];
float      delay;
int        thrdid;
static int mesg         = 0;

Edbg(("thrdWriter(%spfd)",pfd? "" : "null "));

/* figure out the "thrdid" */
for(thrdid= 0; thrdid < thrdmax; ++thrdid) if(pthread_self() == thrd[thrdid]) break;

printf("writer thread#%-2d: (%2d readers, %2d writers) attempting to acquire rwlock\n",thrdid,rctr,wctr);
pthread_rwlock_wrlock(&rwlock);
++wctr;
lastwrite= lseek(*pfd,0,SEEK_CUR);
if(lastwrite < 0) {
	perror(strprintf("writer thread#%d",thrdid));
	}
sprintf(buf,"-- thread#%-2d message#%-2d--\n",thrdid,++mesg);
write(*pfd,buf,strlen(buf)+1);
printf("writer thread#%-2d: (%2d readers, %2d writers) wrote message<%s> at %ld\n",thrdid,rctr,wctr,buf,lastwrite);
delay= rndm()*3000000.;
usleep((useconds_t) delay); /* wait some amount of time between [0,3] seconds */
printf("writer thread#%-2d: (%2d readers, %2d writers) releasing rwlock after %9.6fsec delay\n",thrdid,rctr,wctr,delay/1000000.);
pthread_rwlock_unlock(&rwlock);
--wctr;
printf("writer thread#%-2d: (%2d readers, %2d writers) rwlock released\n",thrdid,rctr,wctr);

Rdbg(("thrdWriter"));
}

/* ===================================================================== */
/* Modelines: {{{1
 *  vim: fdm=marker fenc=utf-8 ts=4
 */
