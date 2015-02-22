/* Jason Quisberth CS507 hw 4
   goal: traverse files in directories using recusion and space padding as per ascension 
   to run gcc -g uplister.c -o uplister && ./uplister 
*/
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>

/* global vars for current working directory and traversal limit*/
int size = 1024;
char cwd[1024];
int travLim = 4;

/* frunction prototype */
int printCon(DIR *ptr, struct dirent *eptr, int *cnt,char *cwdPtr);


int main(int argc, char *argv[]) {
	/* variable declarations for a directory stream, directory entry, and recursion counter */
	DIR *dir = NULL;
	struct dirent *entry = NULL;
	int count = 0;

	getcwd(cwd, sizeof(cwd));	

	dir = opendir(cwd);
	/* recursive function call */
	printCon(dir, entry, &count, cwd);

	/* close directory stream */
	closedir(dir);
	return 0;
}

/* recursive function to print contents of directory */
int printCon(DIR *ptr, struct dirent *eptr, int *cnt, char *cwdPtr){

	/* check for null ptr or if traversal limit is hit to stop recursion */
	if(ptr == NULL || *cnt == travLim){
		if(ptr == NULL)
			perror("null ptr");
		printf("traversal complete!\n");
		return 0; 
	}

	/* eptr->d_type: 4 = dir 8 = file */
	while((eptr= readdir(ptr)) != NULL) {
		//printf("is it a file? %d\n",eptr->d_type);
		if(eptr->d_type == 8)
			printf("%*s\n", (*cnt*2)+(int)strlen(eptr->d_name), eptr->d_name);
			/* ^^ print two spaces per directory level ascension */
	}
	
	/* close stream to open another about the parent directory */
	closedir(ptr);

	cwdPtr = strcat(cwd,"/..");
	ptr = opendir(cwdPtr);
	getcwd(cwdPtr, sizeof(cwdPtr));	

	*cnt = *cnt + 1;

	return printCon(ptr, eptr, cnt, cwdPtr);
}
