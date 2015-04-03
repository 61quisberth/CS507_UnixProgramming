/* Jason Quisberth CS507 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>

int main(int argc, char *argv[])
{
        int     fd[2], nbytes;
        pid_t   child1, child2;
        char    readbuffer[80];
        char    childreadbuffer[80];

        pipe(fd);
        
	/* if no arguments are given: provide instructons */
	if (argc != 2) {
		fprintf(stderr, "Usage: %s <string>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	/* gen first child */
        if((child1 = fork()) == -1)
        {
                perror("fork");
                exit(1);
        }

	/* gen second child */
        if((child2 = fork()) == -1)
        {
                perror("fork");
                exit(1);
        }

        if(child1 == 0)
        {
		close(fd[0]);          /* Close unused read end */
                /* Read in a string from the pipe */
                nbytes = read(fd[0], readbuffer, sizeof(readbuffer));
                /* Send "string" through the output side of pipe */
                write(fd[1], readbuffer, (strlen(readbuffer)+1));
                exit(0);
        }

        if(child2 == 0)
        {
                /* Child process closes up input side of pipe */
                close(fd[1]);
                /* Read in a string from the pipe */
                nbytes = read(fd[0], childreadbuffer, sizeof(childreadbuffer));
                printf("Received string: %s\n", childreadbuffer);
                exit(0);
        }
        else
        {
		close(fd[0]);    /* parent closes input side */ 
		write(fd[1], argv[1], strlen(argv[1])+1);
                close(fd[1]); /* parent closes output side  */
		wait(NULL);  /* wait for child */
		exit(EXIT_SUCCESS);

        }
        
        return 0;
}
