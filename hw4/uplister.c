/* hw 4 Jason Quisberth Unix Systems Programming CUA  
http://www.drchip.org/astronaut/cua/csc507/class04/hw04.pdf
*/
#include <stdio.h>
#include <errno.h>
#include <dirent.h> // directory header


/* required functions opendir, readdir, and closedir 
two space indent required per new directory 
*/

void printCon(DIR *ptr, struct dirent *eptr);

int main() {
	/* create ptr to dir*/
	DIR *dirPtr = NULL;
	/* directory entry ptr */
	struct dirent *dirEntryPtr = NULL;
	
	printCon(dirPtr, dirEntryPtr);	
	closedir(dirPtr);
	return 0;
}


void printCon(DIR *ptr, struct dirent *eptr){

	ptr = opendir("."); /* current directory */
	
	if(ptr == NULL)
		perror("ERROR: Directory ptr error");

	while(eptr = readdir(ptr) ){ /* as long as files are in dir not read */
		if(eptr== NULL)
			perror("ERROR: Directory Entry ptr error");
		printf("%s\n", eptr->d_name);
	}

}

