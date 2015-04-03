/* Jason Quisberth CS 507 
* homework 7
*/

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include "xtdio.h"

int main(int argc, char **argv){
	pid_t pid;

	if((pid= fork()) > 0) { /* parent process */
		pid_t childpid;
		int status;
		printf("I'm the parent process: pid=%xx  ppid=%xx pgrp=%xx pgid=%xx\n",getpid(),getppid(),getpgrp(),getpgid(getpid()));
		childpid= wait(&status);
		exit(0);
	}
	else if(pid == 0) {     /* child process  */
		printf("I'm the child  process: pid=%xx  ppid=%xx pgrp=%xx pgid=%xx\n",getpid(),getppid(),getpgrp(),getpgid(getpid()));
		exit(0);
	}
	else error(XTDIO_ERROR,"fork error: <%s>\n",strerror(errno));

	return 0;
}
