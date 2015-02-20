/* example code to print contents of dir 
http://www.dreamincode.net/forums/topic/59943-accessing-directories-in-cc-part-i/
*/
#include <dirent.h> // directory header
#include <stdio.h> // printf()
#include <stdlib.h> // exit()

int main () // entry point of the program
{
    // first off, we need to create a pointer to a directory
	DIR *pdir = NULL; // remember, it's good practice to initialise a pointer to NULL!
	struct dirent *pent = NULL;

    // I used the current directory, since this is one which will apply to anyone reading
    // this tutorial~ If I said "C:\\" and you're on Linux, it may get a little confusing!
    pdir = opendir ("."); // "." will refer to the current directory
    if (pdir == NULL) // if pdir wasn't initialised correctly
    { // print an error message and exit the program
        printf ("\nERROR! pdir could not be initialised correctly");
        exit (3);
    } // end if

    while (pent = readdir (pdir)) // while there is still something in the directory to list
    {
        if (pent == NULL) // if pent has not been initialised correctly
        { // print an error message, and exit the program
            printf ("ERROR! pent could not be initialised correctly");
            exit (3);
	}
        // otherwise, it was initialised correctly. let's print it on the console:
        printf ("%s\n", pent->d_name);
    }

    // finally, let's close the directory
    closedir (pdir);

    return 0; // everything went OK
}
