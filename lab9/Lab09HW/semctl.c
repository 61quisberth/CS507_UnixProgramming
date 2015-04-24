/* semctl.c: this program illustrates the semaphore control semctl()
 * system call capabilities
 */

/* =====================================================================
 * Header Section: {{{1
 */
#include <stdio.h>
#ifndef __USE_SVID
# define __USE_SVID
#endif
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include "xtdio.h"

/* --------------------------------------------------------------------- */
/* main: {{{1 */
int main(int argc,char **argv)
{
extern int      errno;
int             c;
int             choice;
int             cmd;
int             gid;
int             i;
int             length;
int             mode;
int             ret;
int             semid;
int             semnum;
int             uid;
struct semid_ds semid_ds;
union semun {
	int              val;
	struct semid_ds *buf;
	ushort          *array;
	} semun;
ushort array[25];

Initdbg((&argc,argv));
rdcolor();

/* Initialize the data structure pointer */
semun.buf= &semid_ds;

/* Enter the semaphore ID */
if(argc > 1) sscanf(argv[1],"%d",&semid);
else {
	printf("%sEnter the semid%s: ",CYAN,GREEN);
	scanf("%d",&semid);
	}

/* Choose the desired command */
printf("\n%sSemaphore Commands%s\n",WHITE,CYAN);
printf("GETVAL     =  1\n");
printf("SETVAL     =  2\n");
printf("GETPID     =  3\n");
printf("GETNCNT    =  4\n");
printf("GETZCNT    =  5\n");
printf("GETALL     =  6\n");
printf("SETALL     =  7\n");
printf("IPC_STAT   =  8\n");
printf("IPC_SET    =  9\n");
printf("IPC_RMID   = 10\n");
printf("%sEnter the desired command number%s: ",CYAN,GREEN);
scanf("%d",&cmd);

/* Check entries */
printf("\nsemid=%d  cmd=%d\n\n",semid,cmd);

/* Set the command and do the semctl call */
switch(cmd) {

case 1:	/* Get a specified value */
	Dprintf((1,"get semaphore value\n"));
	printf("\n%sEnter the semnum%s: ",CYAN,GREEN);
	scanf("%d",&semnum);
	ret= semctl(semid,semnum,GETVAL,0); /* Do the system call */
	printf("issued semctl(semid=%d,semnum=%d,GETVAL,0)\n",semid,semnum);
	printf("%sThe semval is %s%d\n",CYAN,GREEN,ret);
	break;

case 2: /* Set a specified value */
	Dprintf((1,"set semaphore value\n"));
	printf("\n%sEnter the semnum=%s ",CYAN,GREEN);
	scanf("%d",&semnum);
	printf("%sEnter the value%s: ",CYAN,GREEN);
	scanf("%d",&semun.val);
	printf("issued semctl(semid=%d,semnum=%d,SETVAL,semun.val=%d)\n",semid,semnum,semun.val);
	ret= semctl(semid,semnum,SETVAL,semun); /* Do the system call (uses semun.val) */
	break;

case 3:	/* Get the process ID */
	Dprintf((1,"get semaphore set#%d's process id\n"));
	printf("issued semctl(semid=%d,0,GETPID,0)\n",semid);
	ret= semctl(semid,0,GETPID,0);
	printf("%sThe sempid is %s%d\n",CYAN,GREEN,ret);
	break;

case 4:	/* Get the number of processes waiting for the semaphore to become
      	 * greater than its current value
      	 */
	Dprintf((1,"get qty processes waiting for semaphroe to increase\n"));
	printf("\n%sEnter the semnum%s: ",CYAN,GREEN);
	scanf("%d",&semnum);
	ret= semctl(semid,semnum,GETNCNT,0);
	printf("issued semctl(semid=%d,semnum=%d,GETNCNT,0)\n",semid,semnum);
	printf("%sThe semncnt is %s%d\n",CYAN,GREEN,ret);
	break;

case 5: /* Get the number of processes waiting for the semaphore value
      	 * to become zero
      	 */
	Dprintf((1,"get qty processes waiting for semaphore to become zero\n"));
	printf("\n%sEnter the semnum%s: ",CYAN,GREEN);
	scanf("%d",&semnum);
	ret= semctl(semid,semnum,GETZCNT,0);
	printf("issued semctl(semid=%d,semnum=%d,GETZCNT,0)\n",semid,semnum);
	printf("%sThe semcnt is %s%d\n",CYAN,GREEN,ret);
	break;

case 6:	/* Get all of the semaphores */
	Dprintf((1,"get all semaphore value\n"));
	/* Get qty of semaphores in semaphore set */
	ret    = semctl(semid,0,IPC_STAT,semun); /* uses semun.buf */
	if(ret == -1) {
		Dprintf((1,"attempt to IPC_STAT\n"));
		goto PROBLEM;
		}
	length = semun.buf->sem_nsems;

	semun.array= array;
    for(i= 0; i < length; ++i) semun.array[i]= 0;

	/* Get and print all semaphores in the specified set */
    ret= semctl(semid,0,GETALL,semun); /* uses semun.array        */
	printf("issued semctl(semid=%d,0,GETALL,semun=[0...0])\n",semid,semnum);
	if(ret == -1) {
		Dprintf((1,"attempted semctl with GETALL\n"));
		goto PROBLEM;
		}
    for(i= 0; i < length; ++i) {
        printf("%hu",semun.array[i]);
		if(i == length-1) printf("\n");
		else              printf(" ");
		}
	break;

case 7:	/* Set all semaphores in the set */
	Dprintf((1,"set all semaphore values\n"));
	/* Get the number of semaphores in the set */
	printf("issued semctl(semid=%d,0,IPC_STAT,semun)\n",semid,semnum);
	ret    = semctl(semid,0,IPC_STAT,semun); /* uses semun.buf */
	length = semun.buf->sem_nsems;

	if(ret == -1) goto PROBLEM;

	/* Set the semaphore set values */
	semun.array= array;
	printf("\n%sEnter %d new semaphore values %s: ",CYAN,length,GREEN);
	for(i= 0; i < length; ++i) {
		scanf("%d",&c);
		semun.array[i]= c;
		}
	printf("issued semctl(semid=%d,0,SETALL,semun=[%d...%d]\n",semid,semnum,c,c);
	ret= semctl(semid,0,SETALL,semun); /* uses semun.array */
	break;

case 8:	/* Get the status for the semaphore set */
	Dprintf((1,"get semaphore set status\n"));
	/* Get and print the current status values */
	printf("issued semctl(semid=%d,0,IPC_STAT,semun)\n",semid,semnum);
	ret= semctl(semid,0,IPC_STAT,semun);	 /* uses semun.buf */
	printf("%sThe USER  ID is                    %s%d\n", CYAN,GREEN,semun.buf->sem_perm.uid);
	printf("%sThe GROUP ID is                    %s%d\n", CYAN,GREEN,semun.buf->sem_perm.gid);
	printf("%sThe operation permissions is       %s%d\n", CYAN,GREEN,semun.buf->sem_perm.mode);
	printf("%sThe number of semaphores in set is %s%ld\n",CYAN,GREEN,semun.buf->sem_nsems);
	printf("%sThe last semop time is             %s%d\n", CYAN,GREEN,(int)semun.buf->sem_otime);
	printf("%sThe last change time is            %s%d\n", CYAN,GREEN,(int)semun.buf->sem_ctime);
	break;

case 9:	/* Select and change the desired member of the data structure */
	Dprintf((1,"select&change semaphore control information\n"));
	/* Get the current status values */
	printf("issued semctl(semid=%d,0,IPC_STAT,semun)\n",semid,semnum);
	ret= semctl(semid,0,IPC_STAT,semun); 	/* uses semun.buf */
	if(ret == -1) goto PROBLEM;

	/* Select the member to be changed */
	printf("%sMember Codes\n",WHITE);
	printf("%ssem_perm.uid  = %s1\n",CYAN,GREEN);
	printf("%ssem_perm.gid  = %s2\n",CYAN,GREEN);
	printf("%ssem_perm.mode = %s3\n",CYAN,GREEN);
	printf("%sSelect the member to be changed%s: ",CYAN,GREEN);
	scanf("%d",&choice);

	switch(choice) {

	case 1:	/* change the user ID */
		printf("%sEnter User ID%s: ",CYAN,GREEN);
		scanf("%d",&uid);
		semun.buf->sem_perm.uid= uid;
		break;

	case 2:	/* change the group id */
		printf("%sEnter Group ID%s: ",CYAN,GREEN);
		scanf("%d",&gid);
		semun.buf->sem_perm.gid= gid;
		break;

	case 3:	/* change the mode */
		printf("%sEnter new Mode%s: ",CYAN,GREEN);
		scanf("%o",&mode);
		semun.buf->sem_perm.mode= mode;
		break;
	default:
		error(XTDIO_ERROR,"illegal choice %d\n",choice);
		break;
		}
	printf("issued semctl(semid=%d,0,IPC_SET,semun)\n",semid,semnum);
	ret= semctl(semid,0,IPC_SET,semun);	/* uses semun.buf */
	break;

case 10:	/* Remove the semid along with its data structure */
	Dprintf((1,"remove semaphore set#%d along with its data\n",semid));
	printf("issued semctl(semid=%d,0,IPC_RMID,0\n",semid);
	ret= semctl(semid,0,IPC_RMID,0);
	break;

default:
	error(XTDIO_ERROR,"illegal semctl command (%s%d%s) entered\n",YELLOW,cmd,GREEN);
	break;
	}

PROBLEM:
if(ret == -1) {
	error(XTDIO_WARNING,"(semctl) system call %sFAILED%s with error #%d<%s>!\n",
	  RED,GREEN,errno,strerror(errno));
	}
else printf("\nSemctl was successful using semid=%d\n",semid);

Rdbg(("main 0"));
return 0;
}

/* ===================================================================== */
/* Modelines: {{{1
 *  vim: fdm=marker fenc=utf-8 ts=4
 */

