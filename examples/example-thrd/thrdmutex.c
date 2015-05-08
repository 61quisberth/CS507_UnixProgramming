/* thrdmutex.c: this program illustrates the (coarse-grained) use of mutexes
 * with a linked list.  mutex == mutual exclusion lock
 *   Author: Charles Campbell
 */

/* =====================================================================
 * Header Section: {{{1
 */

/* ---------------------------------------------------------------------
 * Includes: {{{2
 */
#include <stdio.h>
#include <pthread.h>
#include "xtdio.h"
#include "xmath.h"

/* ---------------------------------------------------------------------
 * Definitions: {{{2
 */
#define MAXCNT 10
#if defined(__linux) || defined(sgi)
/*# define ThreadYield()	sched_yield()*/
# define ThreadYield()
#else
# define ThreadYield()	thr_yield()
#endif

/* ---------------------------------------------------------------------
 * Typedefs: {{{2
 */
typedef struct NameLL_str     NameLL;
typedef struct NameElemLL_str NameElemLL;

/* ---------------------------------------------------------------------
 * Structures: {{{2
 */
struct NameLL_str {
	pthread_mutex_t  mutex;
	NameElemLL      *head;
	NameElemLL      *tail;
	};

struct NameElemLL_str {
	char       *name;
	NameElemLL *nxt;
	NameElemLL *prv;
	};

/* ---------------------------------------------------------------------
 * Prototypes: {{{2
 */
void procA(int *);
void procB(int *);

/* ---------------------------------------------------------------------
 * Global Data: {{{2
 */
NameLL namelist;
char   ctr = 0;
int    resA= 0;
int    resB= 0;
long   s1;
long   s2;
long   s3;

/* =====================================================================
 * Functions: {{{1
 */

/* --------------------------------------------------------------------- */
/* main: begin here as usual {{{2 */
int main(int argc,char **argv)
{
pthread_t   thrdA;
pthread_t   thrdB;
NameElemLL *name;

init_rndm(&s1,&s2,&s3);

pthread_mutex_init(&(namelist.mutex),NULL);

pthread_create(
  &thrdA,			/* pthread_t                        */
  NULL,				/* attribute                        */
  (void *) procA,	/* starting routine for this thread */
  (void *) &resA);	/* argument supplied to this thread */
pthread_create(
  &thrdB,			/* pthread_t                        */
  NULL,				/* attribute                        */
  (void *) procB,	/* starting routine for this thread */
  (void *) &resB);	/* argument supplied to this thread */

pthread_join(thrdA,NULL);
pthread_join(thrdB,NULL);

/* print out the list */
for(name= namelist.head; name; name= name->nxt) {
	printf("name<%s>\n",name->name);
	}

return 0;
}

/* ---------------------------------------------------------- */
/* procA: {{{2 */
void procA(int *inpA)
{
char   buf[5];
double r;

while(ctr < MAXCNT) {
	r= rndm();
	printf("A ctr=%d r=%.3f\n",ctr,r);
	if(r < .5) {
		sprintf(buf,"A%d",ctr++);
		pthread_mutex_lock(&(namelist.mutex));
		double_link(NameElemLL,namelist.head,namelist.tail,"Failed to allocate a NameElemLL");
		stralloc(namelist.tail->name,buf,"Failed to allocate a string");
		pthread_mutex_unlock(&(namelist.mutex));
		}
	else ThreadYield();
	}
pthread_exit(NULL);
}

/* ---------------------------------------------------------- */
/* procB: {{{2 */
void procB(int *inpB)
{
char   buf[5];
double r;

while(ctr < MAXCNT) {
	r= rndm();
	printf("B ctr=%d r=%.3f\n",ctr,r);
	if(r < .5) {
		sprintf(buf,"B%d",ctr++);
		pthread_mutex_lock(&(namelist.mutex));
		double_link(NameElemLL,namelist.head,namelist.tail,"Failed to allocate a NameElemLL");
		stralloc(namelist.tail->name,buf,"Failed to allocate a string");
		pthread_mutex_unlock(&(namelist.mutex));
		}
	else ThreadYield();
	}
pthread_exit(NULL);
}

/* ===================================================================== */
/* Modelines: {{{1
 *  vim: fdm=marker
 */
