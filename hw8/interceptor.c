// CS 507 Jason Quisberth hw 8
#include <stdio.h>
#include <signal.h>
#include <string.h>   /* for memset */
#include <unistd.h>   /* for sleep and getpid */

int done = 0;

void sighandler(int signo)
{
	printf("Received signal %d.\n",signo);
}

int main(void)
{
	struct sigaction act;
	char strExit[] = "quit";
	char str[4];

	memset(&act, 0, sizeof(act));
	act.sa_handler = sighandler;

	int i;
	for(i = 1; i < 51;i++)
		sigaction(i,&act, 0);
	

	printf("Send signalt to pid: %d or type 'quit' to exit.\n", getpid());

	while (!done){
		fgets(str,4,stdin);
		if(strcmp(strExit,str) == 0)
			done = 1;
	}

	printf("All done!\n");

	return 0;
}
