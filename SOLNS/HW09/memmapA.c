/* memmapA.c: this program implements hw#09
 *   Author: Charles E. Campbell
 *   Date:   Apr 18, 2011
 */

/* =====================================================================
 * Header Section: {{{1
 */

/* ---------------------------------------------------------------------
 * Includes: {{{2
 */
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "xtdio.h"

/* ------------------------------------------------------------------------
 * Definitions: {{{2
 */
#define QTYSEM	2
#define MEMBUF	256
#define PATH_MAX 256

/* ------------------------------------------------------------------------
 * Typedefs: {{{2
 */

/* ------------------------------------------------------------------------
 * Local Data Structures: {{{2
 */

/* ------------------------------------------------------------------------
 * Global Data: {{{2
 */
char *membuf = NULL; /* this is the array that will be memory mapped */
int   semA   = 0;
int   semB   = 1;
int   semid;

/* ------------------------------------------------------------------------
 * Explanation: {{{2
 */

/* ------------------------------------------------------------------------
 * Prototypes: {{{2
 */
int main( int, char **);        /* memmapA.c */
void SendArgs(int,int,char **); /* memmapA.c */
void RcvArgs(int);              /* memmapA.c */

/* ========================================================================
 * Functions: {{{1
 */

/* --------------------------------------------------------------------- */
/* main: {{{2 */
int main(
  int    argc,
  char **argv)
{
key_t        ipckey;
static char  path[PATH_MAX+1];
static char  memmapPath[PATH_MAX+1];
ushort       array[QTYSEM];                 /* used to initialize semaphores */
int          IamA                   = 0;    /* 0=memmapB 1=memmapA           */
int          IdidInit               = 0;
int          fd;
char        *mb                     = NULL;

Initdbg((&argc,argv));
rdcolor();

/* get a key */
getcwd(path,PATH_MAX+1);
sprintf(memmapPath,"%s/memmapA",path);
ipckey = ftok(memmapPath,50);

/* get semaphore set */
semid  =  semget(ipckey,QTYSEM,IPC_CREAT|IPC_EXCL);
if(semid == -1) {
	IdidInit = 0;
	semid    = semget(ipckey,QTYSEM,IPC_CREAT);
	if(semid == -1) { /* whoops! */
		perror("2nd semget failed: ");
		exit(-1);
		}
	fd= open("memmapfile",O_CREAT|O_RDWR,S_IRUSR|S_IWUSR);
	if(fd == -1) {
		perror("memmapfile open failed: ");
		exit(-1);
		}
	}
else { /* first time */
	IdidInit= 1;
	printf("did init\n");

	/* initialize the semaphores: memmapA uses [0,1] and memmapB uses [1,0] */
	if(argv[0][strlen(argv[0])-1] == 'A') {
		array[0] = 1; /* semA can be decremented which allows memmapA to write           */
		array[1] = 0; /* semB can NOT be decremented which prevents memmapB from writing */
		IamA     = 1;
		}
	else {
		array[0] = 0; /* semA can NOT be decremented which prevents memmapA from writing */
		array[1] = 1; /* semB can be decremented which allows memmapB to write           */
		IamA     = 0;
		}
	semctl(semid,0,SETALL,array);

	/* initialize the file by writing a byte at offset=MEMBUF-1 */
	fd= open("memmapfile",O_CREAT|O_RDWR|O_EXCL,S_IRUSR|S_IWUSR);
	if(fd == -1) {
		perror("memmapfile open failed: ");
		semctl(semid,0,IPC_RMID);           /* remove semaphores */
		exit(-1);
		}
	lseek(fd,(long)(MEMBUF-1),SEEK_SET);
	if(write(fd,"",1) == -1) {
		perror("unable to write to end-of-file: ");
		semctl(semid,0,IPC_RMID);           /* remove semaphores */
		exit(-1);
		}
	}

/* memory map the file to the membuf */
Dprintf((1,"memory map file to membuf  (fd=%d)\n",fd));
membuf = mmap(NULL,(size_t)MEMBUF,PROT_READ|PROT_WRITE,MAP_SHARED,fd,(off_t)0);
if(membuf == ((void *)-1)) {
	perror("attempt to mmap: ");
	semctl(semid,0,IPC_RMID);           /* remove semaphores */
	exit(-1);
	}
printf("membuf is now memory mapped  (mb=%px  membuf=%px)\n",mb,membuf);

/* attempt to get access to shared memory file */
if(IamA && IdidInit) {
	SendArgs(semA,argc,argv);
	printf("waiting for B to start\n");
	sleep(5);
	RcvArgs(semA);
	}
else if(IamA && !IdidInit) {
	RcvArgs(semA);
	SendArgs(semA,argc,argv);
	}
else if(!IamA &&  IdidInit) {
	SendArgs(semB,argc,argv);
	printf("waiting for A to start\n");
	sleep(5);
	RcvArgs(semB);
	}
else if(!IamA && !IdidInit) {
	RcvArgs(semB);
	SendArgs(semB,argc,argv);
	}

/* do cleanup */
if(IdidInit) {
	Dprintf((1,"do cleanup (removing file<%s>)\n",sprt(memmapPath)));
	sleep(2);                 /* give the receiver some time to finish up */
	unlink(memmapPath);       /* remove shared memory file                */
	semctl(semid,0,IPC_RMID); /* remove semaphores                        */
	printf("did cleanup\n");
	}

Rdbg(("main 0"));
return 0;
}

/* --------------------------------------------------------------------- */
/* SendArgs: this function sends command line strings to other process via shared memory {{{2 */
void SendArgs(int sem,int argc,char **argv)
{
struct sembuf sops[2];
int iarg;
char *mb;

Edbg(("SendArgs(sem=%d<%s>,argc=%d,%sargv)",
  sem,
  (sem == semA)? "A" : "B",
  argc,
  argv? "" : "null "));

sops[0].sem_num = sem;
sops[0].sem_op  = -1; /* decrements sem[AB]'s semaphore */
sops[0].sem_flg = 0;  /* block if unavailable           */
semop(semid,(void *)&sops,(size_t) 1);

	/* sem is now zero and hence memmap[AB] can write to it */
	/* send message                                         */
	for(iarg= 1, mb= membuf; iarg < argc; ++iarg) {
		strcpy(mb,argv[iarg]);
		mb+= strlen(mb);
		strcpy(mb," ");
		mb+= strlen(mb);
		}
	printf("sent <%s>\n",membuf);

	sops[0].sem_num = semA;
	sops[0].sem_op  = 1;    /* increments semA's semaphore */
	sops[0].sem_flg = 0;    /* block if unavailable        */
	sops[1].sem_num = semB;
	sops[1].sem_op  = 1;    /* increments semB's semaphore */
	sops[1].sem_flg = 0;    /* block if unavailable        */
semop(semid,(void *)&sops,(size_t) 2);

Rdbg(("SendArgs"));
}

/* --------------------------------------------------------------------- */
/* RcvArgs: this function displays other process's command line arguments via shared memory {{{2 */
void RcvArgs(int sem)
{
struct sembuf sops[2];

Edbg(("RcvArgs(sem=%d<%s>)",
  sem,
  (sem == semA)? "A" : "B"));

sops[0].sem_num = !sem;
sops[0].sem_op  = -1; /* decrements sem's semaphore */
sops[0].sem_flg = 0;  /* block if unavailable       */
semop(semid,(void *)&sops,(size_t) 1);
	/* sem is now zero and hence memmap can read from it */
	printf("rcvd <%s>\n",membuf);

	sops[0].sem_num = !sem;
	sops[0].sem_op  = 1; /* increments sem's semaphore */
	sops[0].sem_flg = 0; /* block if unavailable       */
semop(semid,(void *)&sops,(size_t) 1);

Rdbg(("RcvArgs"));
}

/* --------------------------------------------------------------------- */
/* Modelines: {{{1
 *  vim: fdm=marker fenc=utf-8 ts=4
 */
