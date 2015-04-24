/* msgget.c: this program illustrates Unix message get: msgget()
 * system call capabilities
 */
#include <stdio.h>
#ifndef __USE_SVID
# define __USE_SVID
#endif
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>

/* --------------------------------------------------------------------- */

/* main: start of main C language program */
int main()
{
key_t key;
int flags;
int msqid;
int opperm;
int opperm_flags;

/* enter the desired key */
printf("Enter the desired key in hex: ");
scanf("%x",&key);

/* Enter the desired octal operation permissions */
printf("\nEnter the operation\n");
printf("permissions in octal: ");
scanf("%o",&opperm);

/* Set the desired flags */
printf("\nEnter corresponding number to\n");
printf("set the desired flags:\n");
printf("No flags                  = 0\n");
printf("IPC_CREAT                 = 1\n");
printf("IPC_EXCL                  = 2\n");
printf("IPC_CREAT and IPC_EXCL    = 3\n");

/* Get the flag(s) to be set */
printf("Enter Flags               : ");
scanf("%d",&flags);

/* Check the values */
printf("\nInput was: key=0x%x  opperm=O%o  flags=O%o\n", key,opperm,flags);

/* Incorporate the control fields (flags) with the operation permissions */
switch(flags) {

case 0:	/* No flags are to be set */
	opperm_flags= (opperm | 0);
	break;

case 1:	/* Set the IPC_CREAT flag */
	opperm_flags= (opperm | IPC_CREAT);
	break;

case 2:	/* Set the IPC_EXCL flag */
	opperm_flags= (opperm | IPC_EXCL);
	break;

case 3:	/* Set the IPC_CREAT and IPC_EXCL flags */
	opperm_flags= (opperm | IPC_CREAT | IPC_EXCL);
	break;
	}

/* Call the msgget system call */
msqid= msgget(key,opperm_flags);

/* Perform the following if the call is unsuccessful */
if(msqid == -1) {
	printf("\nThe msgget system call failed with error %d\n",errno);
	perror("msgget");
	}
else {	/* Return the msqid upon successful completion */
	printf("\nThe msqid is %d\n",msqid);
	}
return 0;
}

/* --------------------------------------------------------------------- */

