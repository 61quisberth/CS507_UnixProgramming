// CS507 Jason Quisberth hw 8
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
 
static int got_signal = 0;
 
static void hdl (int sig)
{
	got_signal = 1;
}
 
int main (int argc, char *argv[])
{
	sigset_t mask;
	sigset_t orig_mask;
	struct sigaction act;
	char strIn[5];
 
	memset (&act, 0, sizeof(act));
	act.sa_handler = hdl;
	
	printf("send me (pid: %d) SIGTERM or quit to exit\n",getpid());
	if( fgets(strIn,5, stdin) != NULL){
		if(strcmp(strIn,"quit") == 0){	
			exit(1);
		}
		else if(strcmp(strIn,"mask")==0){
			printf("enter what signals to mask: \n");
		}
		else if(strcmp(strIn,"umsk")==0){
			printf("enter what signals to unmask: \n");
		}
	}
 
	if (sigaction(SIGTERM, &act, 0)) {
		perror ("sigaction");
		return 1;
	}
 
	sigemptyset (&mask);
	sigaddset (&mask, SIGTERM);
 
	if (sigprocmask(SIG_BLOCK, &mask, &orig_mask) < 0) {
		perror ("sigprocmask");
		return 1;
	}
 
	//sleep (10);
 
	if (sigprocmask(SIG_SETMASK, &orig_mask, NULL) < 0) {
		perror ("sigprocmask");
		return 1;
	}
 
	//sleep (1);
 
	if (got_signal)
		printf("Got signal\n");
 
	return 0;
}
