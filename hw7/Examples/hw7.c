/* Jason Quibserth CS 507 Catholic University of America 
   Homework #7
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include "xtdio.h"
#include <signal.h>

/* handler function for signal*/
/*
static void handler(int sig){
	printf("handling!\n");
}
*/

int main(int argc, char *argv[]){
	pid_t pid;
	pid_t pid0;
	//signal(SIGUSR1, handler);

	if((pid= fork()) > 0) { /* parent process */
		pid_t childpid;
		int status;
		printf("I'm the parent process: pid=%d  ppid=%d pgrp=%d pgid=%d\n",getpid(),getppid(),getpgrp(),getpgid(getpid()));
		//kill(pid, SIGUSR1);
		childpid= wait(&status);
		exit(0);
	}
	else if(pid == 0) {     /* child process  */
		if((pid= fork()) > 0) { /* parent process */
			pid_t childpid;
			int status;
			printf("I'm the child process: pid=%d  ppid=%d pgrp=%d pgid=%d\n",getpid(),getppid(),getpgrp(),getpgid(getpid()));
			char *command = "./hw7c1";
			char *arguments[] = {"./hw7c1","1",NULL};
			execvp(command, arguments);
			childpid= wait(&status);
			exit(0);
		}

		printf("I'm the grandchild process: pid=%d  ppid=%d pgrp=%d pgid=%d\n",getpid(),getppid(),getpgrp(),getpgid(getpid()));
		//execl("./hw7c2","hw7c2",0,(char* )0);
		char *command = "./hw7c2";
		char *arguments[] = {"./hw7c2","2",NULL};
		execvp(command, arguments);
		exit(0);
	}
	else error(XTDIO_ERROR,"fork error: <%s>\n",strerror(errno));

	return 0;
}
