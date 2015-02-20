/* hw 4 Jason Quisberth Unix Systems Programming CUA  
http://www.drchip.org/astronaut/cua/csc507/class04/hw04.pdf
*/
#include <unistd.h>
#include <stdio.h>
#include <errno.h>


/* required functions opendir, readdir, and closedir 
two space indent required per new directory 

possible strategy: use chdir() to change directories working on 
*/

void printCon(char cwd[], int size);

int main() {
	char cwd[1024];
	printCon(cwd,sizeof(cwd));
	return 0;
}

void printCon(char cwd[], int size){
	if (getcwd(cwd,size) != NULL){ /*if current working directory is valid */
		fprintf(stdout, "Current working dir: %s \n", cwd);
	}
	else
		perror("getcwd() error");

	chdir("..");
}
