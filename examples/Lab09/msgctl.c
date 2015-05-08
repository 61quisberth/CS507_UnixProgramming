/* msgctl.c: this program illustrates Unix message control: msgctl()
 *           system call capabilities
 */

/* =====================================================================
 * Header Section: {{{1
 */

/* ---------------------------------------------------------------------
 * Includes: {{{2
 */
#include <stdio.h>
#ifndef __USE_SVID
# define __USE_SVID
#endif
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>

/* =====================================================================
 * Functions: {{{1
 */

/* --------------------------------------------------------------------- */
/* main: start of main C language program {{{2 */
int main(int argc, char **argv)
{
int                     uid;
int                     gid;
int                     mode;
int                     bytes;
int                     rtrn;
int                     msqid;
int                     command;
int                     choice;
static struct msqid_ds  msqid_ds;
struct msqid_ds        *buf      = &msqid_ds;

/* Get the msqid and command */
if(argc > 1) sscanf(argv[1],"%d",&msqid);
else {
	printf("Enter the msqid: ");
	scanf("%d",&msqid);
	}

printf("\nEnter the number for the desired command\n");
printf("IPC_STAT    = 1\n");
printf("IPC_SET     = 2\n");
printf("IPC_RMID    = 3\n");
printf("Enter       : ");
scanf("%d",&command);

/* Check the values */
printf("\nInput was: msqid=%d  command=%d\n",msqid,command);

switch(command) {

/* Use msgctl() to duplicate the data structure for msqid in the msqid_ds
 * area pointed to by buf and then print it out
 */
case 1:
	rtrn= msgctl(msqid,IPC_STAT,buf);
	printf("\nThe USER  ID              is %d\n" , buf->msg_perm.uid);
	printf("\nThe GROUP ID              is %d\n" , buf->msg_perm.gid);
	printf("\nThe operation permissions is O%o\n", buf->msg_perm.mode);
	printf("\nThe msg_qbytes            is %d\n" , buf->msg_qbytes);
	break;

/* Select and change the desired member(s) of the data structure */
case 2:
	/* Get the original data for this msqid data structure, FIRST */
	rtrn= msgctl(msqid,IPC_STAT,buf);
	printf("\nEnter the number for the member to be changed\n");
	printf("msg_perm.uid  = 1  (now %d)\n",buf->msg_perm.uid);
	printf("msg_perm.gid  = 2  (now %d)\n",buf->msg_perm.gid);
	printf("msg_perm.mode = 3  (now %d)\n",buf->msg_perm.mode);
	printf("msg_qbytes    = 4  (now %d)\n",buf->msg_qbytes);

	scanf("%d",&choice);

	/* Only one choice is allowed per pass as an illegal entry will
	 * cause repetitive failures until msqid_ds is updated with IPC_STAT
	 */
	switch(choice) {

	case 1:
		printf("\nEnter USER ID: ");
		scanf("%d",&uid);
		buf->msg_perm.uid= uid;
		printf("\nUSER ID= %d\n",buf->msg_perm.uid);
		break;

	case 2:
		printf("\nEnter GROUP ID: ");
		scanf("%d",&gid);
		buf->msg_perm.gid= gid;
		printf("\nGROUP ID= %d\n",buf->msg_perm.gid);
		break;

	case 3:
		printf("\nEnter MODE: ");
		scanf("%o",&mode);
		buf->msg_perm.mode= mode;
		printf("\nMODE= O%o\n",buf->msg_perm.mode);
		break;

	case 4:
		printf("\nEnter msg_qbytes: ");
		scanf("%d",&bytes);
		buf->msg_qbytes= bytes;
		printf("\nmsg_qbytes=%d\n",buf->msg_qbytes);
		break;
		}

	/* Do the change */
	rtrn= msgctl(msqid,IPC_SET,buf);
	break;

/* Remove the msqid along with its associated message queue and
 * data structure
 */
case 3:
	rtrn= msgctl(msqid,IPC_RMID,NULL);
	break;
	}

/* Perform the following if the call is unsuccessful */
if(rtrn == -1) {
	printf("\nThe msgctl system call failed with error %d\n",errno);
	perror("msgctl");
	}
else {	/* Return the msqid upon successful completion */
	printf("\nMsgctl was successful for msqid=%d\n",msqid);
	}
return 0;
}

/* ===================================================================== */
/* Modelines: {{{1
 *  vim: fdm=marker fenc=utf-8 ts=4
 */

