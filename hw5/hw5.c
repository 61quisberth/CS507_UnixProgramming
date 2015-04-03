#include <stdio.h>
#include <fcntl.h>

main()
{
	FILE *fp;
	char buff[255]; /* holds strings from fgets() */
	char buff0[255]; /* hods locked characters */
	char buff1[255]; /* holds unlocked characters */

	struct flock fl = {F_WRLCK, SEEK_CUR, 0, 0, 0};
	fl.l_pid = getpid();


	fp = fopen("./test.txt", "r");

	while(fgets(buff, 255, (FILE*)fp)){
		//fgets(buff, 255, (FILE*)fp);
		/* source for using fopen and fcntl http://stackoverflow.com/questions/7573282/how-to-lock-files-in-c-c-using-fopen */
		if (fcntl(fileno(fp), F_GETLK, &fl) == 0){
				
			printf("found lock:\n");	
		}
		else
			printf("not locked: %s",buff);
	}

		
	/* rewind buffer
	rewind(fp);
	 */

	fclose(fp);
}
