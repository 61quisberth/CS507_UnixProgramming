/* vaccuum cleaner code in c
done on a mac 
by Jason Quisberth
to compile while ignoring errors (compiler thinks goRight is a possible excape character)
cc -w hw2.c
*/

#include <stdio.h>

int main(int argc, char *argv[]){

	int cycles = 2;
	int maxLine = 10;
	int qtyCol = atoi(argv[1]);
	int qtyRow = atoi(argv[2]);
	// strings end with a null byte in C
	char Right[] = "\==>";
	char Left[] = "<==/";
	char *goRight;
	
	printf("command line args: %d\n",argc-1);

	// loop to print command line inputs
	for (int i = 1; i < argc; i++){
		printf("%s ",argv[i]);	
		if (i == argc-1)
			printf("\n");
	}

	// print row and col
	printf("num cols: %d\n",qtyCol);
	printf("num rows: %d\n",qtyRow);

	printf("Hello world\n");

	return 0;
}

