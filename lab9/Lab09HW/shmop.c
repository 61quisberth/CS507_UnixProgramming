/* shmop.c: this is a program to illustrate the shared-memory-operations,
 * shmat() and shmdt(),
 * and saving/storing data into the shared memory segment
 *
 * Enter desired value for shmaddr (0 means let o/s determine it)
 * Specify flags: SHM_RND                 (this plus non-zero shmaddr means round to SHMLBA)
 *                SHM_RDONLY              (read only)
 *                SHM_RND and SHM_RDONLY  (read only and shmaddr rounding)
 *
 * 1: use the shmget program to set up the shared memory
 * 2: program attaches to the shared memory segment
 * 3: requests a string to store there
 * 4: detaches from the shared memory segment
 */

/* =====================================================================
 * Header Section: {{{1
 */

/* ---------------------------------------------------------------------
 * Includes: {{{2
 */
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#ifndef __USE_SVID
# define __USE_SVID
#endif
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>

/* ---------------------------------------------------------------------
 * Definitions: {{{2
 */
#define BUFSIZE	256

/* =====================================================================
 * Functions: {{{1
 */

/* --------------------------------------------------------------------- */
/* main: begin here... {{{2 */
int main(int argc,char **argv)
{
char       *shmaddr = NULL;
const void *addr    = NULL;
int         flags;
void       *pret;
int         iret;
int         shmflg;
int         shmid;

/* Enter the shared memory ID */
if(argc > 1) {
	sscanf(argv[1],"%d",&shmid);
	printf("Using shared memory id#%d\n",shmid);
	}
else {
	printf("Enter the shmid of the shared memory segment to be operated on: ");
	scanf("%d",&shmid);
	}

/* Enter the value for shmaddr */
printf("\nEnter the value for the shared memory address shmaddr (in hex): ");
scanf("%p",&addr);
printf("The desired address is 0x%p\n",addr);

/* Specify the desired flags */
printf("\nEnter the corresponding # for the desired flags\n");
printf("SHM_RND                = 1\n");
printf("SHM_RDONLY             = 2\n");
printf("SHM_RND and SHM_RDONLY = 3\n");
printf("  Enter flag #         : ");
scanf("%d",&flags);

/* ----------------------------
 * Attach shared memory segment
 * ---------------------------- */
switch(flags) {
case 1:
	shmflg= SHM_RND;
	break;

case 2:
	shmflg= SHM_RDONLY;
	break;

case 3:
	shmflg= SHM_RND | SHM_RDONLY;
	break;
	}
printf("\nFlags= 0%o =%s%s\n",
  shmflg,
  (shmflg & SHM_RND)?    " SHM_RND"    : "",
  (shmflg & SHM_RDONLY)? " SHM_RDONLY" : "");

pret= shmat(shmid,(void *) addr,shmflg); /* Do the shmat system call */
if(pret == (void *) -1) {
	printf("\nShmat failed with error=%d\n",errno);
	perror("shmat");
	exit(1);
	}
else {
	shmaddr= (char *) pret;
	printf("\nShmat was successful: shmid=%d, address=0x%p\n",shmid,pret);
	}

if(shmaddr) {
	char  buf[BUFSIZE];
	char *b;

	/* throw away newline left by the scanfs above */
	fgets(buf,BUFSIZE,stdin);
	do {
		printf("Enter string to enter into shared memory or enter \"?\" for query\n");
		fgets(buf,BUFSIZE,stdin);
		for(b= buf + strlen(buf) - 1; b >= buf && isspace(*b); --b) *b= '\0'; /* remove trailing whitespace */
		} while(!buf[0]);

	if(!strcmp(buf,"?")) { /* display what's in shared memory */
		printf("displaying what's in shared memory:\n");
		printf("at 0x%px<%s>\n",shmaddr,shmaddr);
		}
	else {	/* store into shared memory */
		printf("copying buf<%s> into shared memory:\n",buf);
		strcpy(shmaddr,buf);
		printf("stored: shmaddr=%pxx<%s>\n",shmaddr,shmaddr);
		}

	/* ----------------------------
	 * Detach shared memory segment
	 * ---------------------------- */
	printf("Now detaching from shared memory...\n");
	iret= shmdt(shmaddr); /* Do the shmdt call */
	if(iret == -1) {
		printf("shmdt failed with error=%d\n",errno);
		perror("shmdt");
		exit(1);
		}
	else printf("\nShmdt was successful for address 0x%p\n",shmaddr);
	}

return 0;
}

/* ===================================================================== */
/* Modelines: {{{1
 *  vim: fdm=marker fenc=utf-8 ts=4
 */
