// ipcs -s to view semaphores
// ipcrm -s semid to remove
// CS 507 Jason Quisberth 

#include<stdio.h>
#include<string.h>
#include<sys/types.h>
#include<sys/sem.h>
#include<sys/ipc.h>
#include<errno.h>
#include<sys/mman.h>
#include<sys/stat.h>
#include<fcntl.h>

int main(int argc, char *argv[]){

	key_t key;
	int opperm;
	int fd;
	int opperm_flags;
	int semid;
	FILE *fp;
	struct stat sbuf;

	extern int      errno;
	int             c;
	int             choice;
	int             cmd;
	int             gid;
	int             i;
	int             length;
	int             mode;
	int             ret;
	int             semnum;
	int             uid;

	if (argc == 1){
		perror("Use: ./memmapC ['set' or 'unset' or 'memset']");
		exit(1);
	}

	struct semid_ds semid_ds;
	union semun {
		int              val;
		struct semid_ds *buf;
		ushort          *array;
	} semun;
	ushort array[25];


	/* Initialize the data structure pointer */
	semun.buf= &semid_ds;

	if(argc > 1 && strcmp(argv[1],"memset")==0){
		fp = fopen("memfile", "w+");
		fprintf(fp, "This is a shared memfile\n");
		fclose(fp);

		if (argc != 2) {
			fprintf(stderr, "usage: mmapdemo offset\n");
			exit(1);
		}

		if ((fd = open("mmapdemo.c", O_RDONLY)) == -1) {
			perror("open");
			exit(1);
		}

		if (stat("mmapdemo.c", &sbuf) == -1) {
			perror("stat");
			exit(1);
		}
	}

	if (argc > 1 && strcmp(argv[1],"set") == 0){
		/* Set a specified value */
		printf("\nEnter the semnum= ");
		scanf("%d",&semnum);
		printf("Enter the value: ");
		scanf("%d",&semun.val);
		printf("issued semctl(semid=%d,semnum=%d,SETVAL,semun.val=%d)\n",semid,semnum,semun.val);
		ret= semctl(semid,semnum,SETVAL,semun); /* Do the system call (uses semun.val) */

		/* creating sem */
		printf("Enter desired key in hex: ");
		scanf("%x",&key);

		printf("Enter permissions in oct: ");
		scanf("%o",&opperm);

		printf("Input was: key=0x%x  opperm=O%o \n",key,opperm);

		opperm_flags = (opperm | IPC_CREAT);

		semid=semget(key,1,opperm_flags);

		/* if not succesfull, throw error */
		if(semid == -1) {
			printf("The semget system call failed with error %d\n",errno);
			perror("semget");
		}
		/* otherwise, provide semid */
		else {  
			printf("\nThe semid is %d\n",semid);
		}
	}
	
	if (argc > 1 && strcmp(argv[1],"unset") == 0){
		printf("Enter the semid: ");
		scanf("%d",&semid);
		semctl(semid,0,IPC_RMID,0);
		printf("sem removed\n");
	}

	return 0;
}
