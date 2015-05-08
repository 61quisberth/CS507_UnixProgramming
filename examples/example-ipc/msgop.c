/* msgop.c: this program illustrates Unix message operations: msgop()
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

/* ---------------------------------------------------------------------
 * Definitions:
 */
#define MSGBUFLEN	8192

/* ---------------------------------------------------------------------
 * Local Data Structures
 */
struct msgbuf1 {
	long mtype;
	char mtext[MSGBUFLEN];
	};

/* ---------------------------------------------------------------------
 * Local Data:
 */
struct msgbuf1 *msgp=NULL;
struct msgbuf1 rcvbuf;
struct msgbuf1 sndbuf;


/* --------------------------------------------------------------------- */

/* main: start of main C language program */
int main()
{
int  i,c,flag,flags,choice;
int  rtrn,msqid,msgsz,msgflg;
long mtype,msgtyp;
static struct msqid_ds msqid_ds;
struct msqid_ds *buf= &msqid_ds;

/* Select the desired operation */
printf("Enter the corresponding code to send or receive a message\n");
printf("Send            = 1\n");
printf("Receive         = 2\n");

printf("Enter code: ");
scanf("%d",&choice);

if(choice == 1) {	/* Send a message */
	msgp= &sndbuf;	/* Point to user send structure */

	printf("\nEnter the msqid of the message queue to handle the message: ");
	scanf("%d",&msqid);

	/* Set the message type */
	printf("\nEnter a positive integer message type (long) for the message: ");
	scanf("%d",&msgtyp);
	msgp->mtype= msgtyp;

	/* Enter the message to be sent */
	printf("\nEnter a message: (CTRL-D terminates)\n");
	for(i= 0; ((c= getchar()) != EOF); ++i) sndbuf.mtext[i]= c;

	/* Determine the message size */
	msgsz= i+1;

	/* Echo the message to send */
	for(i= 0; i < msgsz; ++i) putchar(sndbuf.mtext[i]);

	/* Set the IPC_NOWAIT flag if desired */
	printf("\nEnter a 1 if you want the IPC_NOWAIT flag set: ");
	scanf("%d",&flag);
	if(flag == 1) msgflg!= IPC_NOWAIT;
	else          msgflg = 0;

	/* Check the msgflg */
	printf("\nInput: msgflg is O%o\n",msgflg);

	/* Send the message */
	rtrn= msgsnd(msqid,(struct msgbuf *) msgp,msgsz,msgflg);

	if(rtrn == -1) printf("\nMsgsnd failed, Error=%d\n",errno);
	else {	/* Print the value of test which should be zero (for succesful) */
		printf("\nValue returned is %d\n",rtrn);

		/* Print the size of the message sent */
		printf("\nMsgsz was %d\n",msgsz);

		/* --- Print out the affected members --- */

		/* Print the incremented number of messages on the queue */
		printf("\nThe msg_qnum is %d\n",buf->msg_qnum);

		/* Print the process id of the last sender */
		printf("The msg_lspid is %d\n",buf->msg_lspid);

		/* Print the last send time */
		printf("The msg_stime is %d\n",buf->msg_stime);
		}
	}

else if(choice == 2) {	/* Receive a message */
	/* Initialize the message pointer to the receive buffer */
	msgp= &rcvbuf;

	/* Specify the message queue which contains the desired message */
	printf("\nEnter the msqid: ");
	scanf("%d",&msqid);

	/* Specify the specific message on the queue by using its type */
	printf("\nEnter the msgtyp: ");
	scanf("%d",&msgtyp);

	/* Configure the control flags for the desired actions */
	printf("\nEnter the corresponding code to select the desired flags\n");
	printf("No flags                   = 0\n");
	printf("MSG_NOERROR                = 1\n");
	printf("IPC_NOWAIT                 = 2\n");
	printf("MSG_NOERROR and IPC_NOWAIT = 3\n");
	printf("Enter Flags: ");
	scanf("%d",&flags);

	switch(flags) {

	case 0:
		msgflg= 0;
		break;

	case 1:
		msgflg|= MSG_NOERROR;
		break;

	case 2:
		msgflg|= IPC_NOWAIT;
		break;

	case 3:
		msgflg|= MSG_NOERROR | IPC_NOWAIT;
		break;
		}

	/* Specify the number of bytes to be received */
	printf("\nEnter the number of bytes to receive (msgsz): ");
	scanf("%d",&msgsz);

	/* Check the values for the arguments */
	printf("\nmsqid  is %d\n",msqid);
	printf("\nmsgtyp is %d\n",msgtyp);
	printf("\nmsgsz  is %d\n",msgsz);
	printf("\nmsgflg is O%o\n",msgflg);

	/* Call msgrcv to receive the message */
	rtrn= msgrcv(msqid,(struct msgbuf *) msgp,msgsz,msgtyp,msgflg);

	if(rtrn == -1) {
		printf("\nMsgrcv failed with error=%d\n",errno);
		perror("msgrcv");
		}
	else {
		printf("\nMsgrcv was successful for msqid=%d\n",msqid);

		/* Print the number of bytes received - it is equal to the returned
		 * value
		 */
		printf("Bytes received=%d\n",rtrn);

		/* Print the received message */
		for(i= 0; i < rtrn; ++i) putchar(rcvbuf.mtext[i]);
		}

	/* Check the associated data structure */
	msgctl(msqid,IPC_STAT,buf);

	/* Print the decremented number of messages */
	msgctl(msqid,IPC_STAT,buf);

	/* Print the process of the last receiver */
	printf("The msg_lrpid=%d\n",buf->msg_lrpid);

	/* Print the last message receive time */
	printf("The msg_rtime=%d\n",buf->msg_rtime);
	}

return 0;
}

/* --------------------------------------------------------------------- */

