/* semop.c: illustrates the semaphore operations available via semop() */
#include <stdio.h>
#ifndef __USE_SVID
# define __USE_SVID
#endif
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>
#include "xtdio.h"

/* --------------------------------------------------------------------- */

#define MAXSTRING	256

/* --------------------------------------------------------------------- */

/* main: begin here... */
int main(int argc,char **argv)
{
char           string[MAXSTRING];
int            flags;
int            i;
int            ret;
int            semid;
int            sem_num;
struct sembuf  sembuf[10];
struct sembuf *sops              = NULL;
unsigned       nsops             = 1;

Initdbg((&argc,argv));
rdcolor();

sops= sembuf;

/* Enter the semaphore ID */
printf("%sEnter the semid of the semaphore set to be operated upon%s: ", CYAN,GREEN);
scanf("%d",&semid);

/* Enter the number of operations */
printf("%sEnter the number of semaphore operations for this set%s: ", CYAN,GREEN);
scanf("%d",&nsops);

/* Initialize the array for the number of operations to be performed */
for(i= 0; i < nsops; ++i, ++sops) {
	if(nsops > 1) printf("%sOperation %s%d%s:\n",CYAN,WHITE,i+1,GREEN);

	/* This determines the semaphore in the semaphore set */
	printf("%sEnter the semaphore number (sem_num)%s: ",CYAN,GREEN);
	scanf("%d",&sem_num);
	sops->sem_num= sem_num;
	printf("you set sem_num to %d\n",sops->sem_num);

	/* Enter a (-)number to determine an unsigned number (no +) to
	 * increment or zero to test for zero.  These values are entered
	 * into a string and converted to integer values
	 */
	printf("%sEnter +number to increment semaphore\n",CYAN);
	printf("%sEnter -number to decrement semaphore\n",CYAN);
	printf("%sEnter  zero   to test for zero\n",CYAN);
	printf("%sEnter the operation for the semaphore (sem_op)%s: ",CYAN,GREEN);
	scanf("%s",string);
	sops->sem_op= atoi(string);
	printf("you set sem_op to %d\n",sops->sem_op);

	/* Specify the desired flags */
	printf("%sFlags\n",WHITE);
	printf("%sNo flags                 %s0\n",CYAN,GREEN);
	printf("%sIPC_NOWAIT               %s1\n",CYAN,GREEN);
	printf("%sSEM_UNDO                 %s2\n",CYAN,GREEN);
	printf("%sIPC_NOWAIT and SEM_UNDO  %s3\n",CYAN,GREEN);
	printf("%sEnter number for desired flag(s)%s: ",CYAN,GREEN);
	scanf("%d",&flags);

	switch(flags) {

	case 0:
		sops->sem_flg= 0;
		break;

	case 1:
		sops->sem_flg= IPC_NOWAIT;
		break;

	case 2:
		sops->sem_flg= SEM_UNDO;
		break;

	case 3:
		sops->sem_flg= IPC_NOWAIT | SEM_UNDO;
		break;

	default:
		error(XTDIO_ERROR,"bad flags (%d)\n",flags);
		break;
		}
	}

/* Print out each structure in the array */
for(i= 0; i < nsops; ++i) {
	printf("\n%ssem_num is   %s%d\n",CYAN,GREEN,sembuf[i].sem_num);
	printf("%ssem_op  is   %s%d\n",CYAN,GREEN,sembuf[i].sem_op);
	printf("%ssem_flg is   %s%oo\n",CYAN,GREEN,sembuf[i].sem_flg);
	}
sops= sembuf;	/* Reset pointer to sembuf[0] */

ret= semop(semid,sops,nsops);
if(ret == -1) {
	error(XTDIO_WARNING,"semop failed with error %d\n",errno);
	perror("semop");
	}
else {
	printf("%sSemop was successful with semid=%s%d\n",YELLOW,CYAN,semid);
	printf("%sValue returned was %s%d\n",CYAN,GREEN);
	}

Rdbg(("main 0"));
return 0;
}

/* --------------------------------------------------------------------- */

