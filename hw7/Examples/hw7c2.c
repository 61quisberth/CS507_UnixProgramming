#include <stdio.h>
#include <signal.h> 
#include <sys/types.h>
#include <unistd.h>

/* handler function for signal*/
static void handler(int sig){
	printf("handling a signal!\n");
}

int main ( int argc, char *argv[] )
{
	if ( argc != 2 ) /* argc should be 2 for correct execution */
	{
		/* We print argv[0] assuming it is the program name */
		printf( "usage: %s number\n", argv[0] );
	}
	else{
		signal(SIGUSR1, handler);
		printf("Hello again input value: %c\n",*argv[1]);
		//kill(pid, SIGUSR1);
		kill(getpid(), SIGUSR1);
	}
	return 0;
}


