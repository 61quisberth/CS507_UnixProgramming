/* shmop.c: this is a program to illustrate the shared memory control
 * shmop() system call capabilities
 */
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#ifndef __USE_SVID
# define __USE_SVID
#endif
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>

#define BUFSIZE	256

/* --------------------------------------------------------------------- */

/* main: begin here... */
int main()
{
char *shmaddr = NULL;
int   addr;
int   attach;
int   detach;
int   flags;
int   i;
int   retrn;
int   shmflg;
int   shmid;

/* Loop for attachments by this process */
printf("Enter the number of attachments for this process (1-4)\n");
printf("  Attachments: ");
scanf("%d",&attach);
printf("Number of attaches is %d\n",attach);

/* sanity maintainer */
if(attach <= 0) attach= 1;
if(attach >  4) attach= 4;

for(i= 1; i <= attach; ++i) {
	/* Enter the shared memory ID */
	printf("\nEnter the shmid of the shared memory segment\n");
	printf("to be operated on: ");
	scanf("%d",&shmid);

	/* Enter the value for shmaddr */
	printf("\nEnter the value for the shared memory address\n");
	printf(" shmaddr (in hex): ");
	scanf("%x",&addr);
	printf("The desired address is 0x%x\n",addr);

	/* Specify the desired flags */
	printf("\nEnter the corresponding # for the desired flags\n");
	printf("SHM_RND                = 1\n");
	printf("SHM_RDONLY             = 2\n");
	printf("SHM_RND and SHM_RDONLY = 3\n");
	printf("  Enter flag #         : ");
	scanf("%d",&flags);

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
	printf("\nFlags is 0%o\n",shmflg);

	/* Do the shmat system call */
	retrn= (int) shmat(shmid,(void *) addr,shmflg);
	if(retrn == -1) {
		printf("\nShmat failed with error=%d\n",errno);
		perror("shmat");
		}
	else {
		shmaddr= (char *) retrn;
		printf("\nShmat was successful: shmid=%d, address=0x%x\n",shmid,retrn);
		}
	}

if(shmaddr) {
	char  buf[BUFSIZE];
	char *b;

	/* throw away newline left by the scanfs above */
	fgets(buf,BUFSIZE,stdin);
	do {
		printf("Enter string to enter into shared memory or\n");
		printf("enter \"?\" for query\n");
		printf("Response: ");
		fgets(buf,BUFSIZE,stdin);
		for(b= buf + strlen(buf) - 1; b >= buf && isspace(*b); --b) *b= '\0';
		} while(!buf[0]);

	if(strcmp(buf,"?")) {	/* store into shared memory */
		strcpy(shmaddr,buf);
		printf("copied <%s> into 0x%px\n",buf,shmaddr);
		}

	/* always print what's in it */
	printf("stored: shmaddr=%pxx<%s>\n",shmaddr,shmaddr);
	}

/* Loop for detachments by this process */
printf("\nEnter the # of detachments for this process (1-4)\n");
printf("  Detachments: ");
scanf("%d",&detach);
printf("Number of detaches is %d\n",detach);

/* sanity maintainer */
if(detach <= 0) detach= 1;
if(detach >  4) detach= 4;

for(i= 1; i <= detach; ++i) {

	/* Enter the value for shmaddr */
	printf("\nEnter the value for the shared memory address\n");
	printf("  shmaddr (in hex): ");
	scanf("%x",&addr);
	printf("The desired address is 0x%x\n",addr);

	/* Do the shmdt call */
	retrn= (int) shmdt((void *) addr);
	if(retrn == -1) {
		printf("shmdt failed with error=%d\n",errno);
		perror("shmdt");
		}
	else {
		printf("\nShmdt was successful for address 0x%x\n",addr);
		}
	}
return 0;
}

/* --------------------------------------------------------------------- */

