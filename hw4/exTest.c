#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>

int main(int argc, char *argv[]) {
    DIR *dir = NULL;
    struct dirent *entry =  NULL;
    struct stat eStat;

 	dir = opendir(".");

    while((entry = readdir(dir)) != NULL) {
        lstat(entry->d_name, &eStat);
        if(S_ISDIR(eStat.st_mode) == 1)
            printf("found directory %s\n", entry->d_name);
        else
            printf("%s is not a dir\n", entry->d_name);
    }

	dir = opendir("..");

    while((entry = readdir(dir)) != NULL) {
        lstat(entry->d_name, &eStat);
        if(S_ISDIR(eStat.st_mode) == 1)
            printf("found directory %s\n", entry->d_name);
        else
            printf("%s is not a dir\n", entry->d_name);
    }



    return 0;
}
