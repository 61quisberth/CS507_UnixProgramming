/* shmctl.c: this is a program to illustrate the shared memory control
 * shmctl() system call capabilities
 */
#include <stdio.h>
#ifndef __USE_SVID
# define __USE_SVID
#endif
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>

/* --------------------------------------------------------------------- */

/* main: begin here... */
int main()
{
int              choice;
int              command;
int              gid;
int              mode;
int              rtrn;
int              shmid;
int              uid;
struct shmid_ds *buf;
struct shmid_ds  shmid_ds;

buf= &shmid_ds;

/* Get the shmid and command */
printf("Enter the shmid: ");
scanf("%d",&shmid);
printf("\nEnter the command number\n");
printf("IPC_STAT       = 1\n");
printf("IPC_SET        = 2\n");
printf("IPC_RMID       = 3\n");
printf("IPC_LOCK       = 4\n");
printf("IPC_UNLOCK     = 5\n");
printf("  Enter cmd #  : ");
scanf("%d",&command);

/* Check the values */
printf("\n  shmid=%d  command=%d\n",shmid,command);

switch(command) {

case 1: /* use shmctl to duplicate the data structure for shmid in the
		 * shmid_ds area pointed to by buf and then print it out
		 */
	rtrn= shmctl(shmid,IPC_STAT,buf);
	printf("\n");
	printf("The user ID                is %d\n",buf->shm_perm.uid);
	printf("The group ID               is %d\n",buf->shm_perm.gid);
	printf("The creator's ID           is %d\n",buf->shm_perm.cuid);
	printf("The creator's group ID     is %d\n",buf->shm_perm.cgid);
	printf("The operation permissions are 0%o\n",buf->shm_perm.mode);
#ifdef sgi
	printf("The slot usage sequence #  is 0x%x\n",buf->shm_perm.seq);
	printf("The key                    is 0x%x = %d\n",
	  buf->shm_perm.key,buf->shm_perm.key);
#endif
	printf("The segment size           is %d\n",buf->shm_segsz);
	printf("The pid of last shmop      is %d\n",buf->shm_lpid);
	printf("The pid of creator         is %d\n",buf->shm_cpid);
	printf("The current # attached     is %d\n",buf->shm_nattch);
#ifdef sgi
	printf("The in-memory # attached   is %d\n",buf->shm_cnattch);
#endif
	printf("The last shmat time        is %d\n",buf->shm_atime);
	printf("The last shmdt time        is %d\n",buf->shm_dtime);
	printf("The last change time       is %d\n",buf->shm_ctime);
	break;

case 2:	/* Select and change the desired member(s) of the data structure */
	/* Get the original data for this shmid data structure first */
	rtrn= shmctl(shmid,IPC_STAT,buf);

	printf("\nEnter the number of the member to be changed\n");
	printf("shm_perm.uid   = 1\n");
	printf("shm_perm.gid   = 2\n");
	printf("shm_perm.mode  = 3\n");
	printf("  Enter #      : ");
	scanf("%d",&choice);

	/* Only one choice is allowed per pass as an illegal entry will
	 * cause repetitive failures until shmid_ds is updated with an
	 * IPC_STAT
	 */
	switch(choice) {

	case 1:
		printf("\nEnter user ID: ");
		scanf("%d",&uid);
		buf->shm_perm.uid= uid;
		printf("\nUser ID is %d\n",buf->shm_perm.uid);
		break;

	case 2:
		printf("\nEnter group ID: ");
		scanf("%d",&gid);
		buf->shm_perm.gid= gid;
		printf("\nGroup ID is %d\n",buf->shm_perm.gid);
		break;

	case 3:
		printf("\nEnter Mode (in octal): ");
		scanf("%o",&mode);
		buf->shm_perm.mode= mode;
		printf("\nMode is 0%o\n",buf->shm_perm.mode);
		break;
	}

	/* Do the change */
	rtrn= shmctl(shmid,IPC_SET,buf);
	break;

case 3:	/* Remove the shmid along with its associated data structure */
	rtrn= shmctl(shmid,IPC_RMID,NULL);
	break;

case 4:	/* Lock the shared memory segment */
	rtrn= shmctl(shmid,SHM_LOCK,NULL);
	break;

case 5:	/* UnLock the shared memory segment */
	rtrn= shmctl(shmid,SHM_UNLOCK,NULL);
	break;
	}

/* Perform the following if the call was unsuccessful */
if(rtrn == -1) {
	printf("The shmctl system call failed with error %d\n",errno);
	perror("shmctl");
	}
else printf("\nShmctl was successful for shmid=%d\n",shmid);
return 0;
}

/* --------------------------------------------------------------------- */

