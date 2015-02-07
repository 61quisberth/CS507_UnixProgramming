/* 
Jason Quisberth CS 507 CUA 
Made on ubuntu 14.04 (but still on a mac)
To compile: gcc hw2.c -o hw2
./hw2 int1 int2
*/

#include <stdio.h>
#include <stdlib.h>
/*included stdlib because of warning from use of calloc source of solution 
http://stackoverflow.com/questions/977233/warning-incompatible-implicit-declaration-of-built-in-function-xyz 
*/
#include <string.h> /* for use of strcpy */
#include <time.h> /* for "random" color gen */

int main(int argc, char **argv){
	/* test if command line arguments are valid */
	if (argc != 3 ){
		printf( "usage: %s filename needs two command line int args (ex: 2 4)\n", argv[0] );
	}
	else{
		int qtyCol = atoi(argv[1]);
		int qtyRow = atoi(argv[2]);
		time_t t;
		srand((unsigned) time(&t)); /* initialize pseudorandom numbers */

		char **goRight;
		char **goLeft;

		goRight = (char **) calloc(qtyCol,sizeof(char *));	
		goLeft = (char **) calloc(qtyCol,sizeof(char *));	

		int i,j,k,m; /* iterators */
		int vaclen = 5; /* leave space for the null byte */
		goRight[0] = (char *) calloc(vaclen,sizeof(char));
		goLeft[0] = (char *) calloc(vaclen,sizeof(char));
		strcpy(goRight[0],"\\==<");
		strcpy(goLeft[0],"<==/");

		/* move cursor to next position to erase */
		printf("\e[0;0H");

		/* populate values for vacuum */
		for(k= 1; k < qtyCol; k++) {
			vaclen++;
			goRight[k]= (char *) calloc(vaclen,sizeof(char));
			goLeft[k]= (char *) calloc(vaclen,sizeof(char));
			sprintf(goRight[k]," %s",goRight[k-1]);
			sprintf(goLeft[k]," %s",goLeft[k-1]);
		}
	
		/* some useful escape sequences 
		\e[A \e[B \e[C \e[D
		up down foreward backward 
		*/	
		/* iterate throughout each row and clean up
		if row is odd, move right, else, move left */
		for(j = 1; j < qtyRow; j++){
			if (j%2 == 1){ /* if odd line, go right */
				for(i= 1; i < qtyCol; i++) {
					printf("\e[1;3%um",(rand() % 7+1)); /* set random color */
					printf("%s\n",goRight[i]);
					printf("\e[1A\e[C"); /* move cursor up and clear line */
					usleep(20000);
					/* if at end of line, go to new line */
					if(i == qtyCol-1)
						printf("\e[K\n");
				}
			}
			else{ /* go left */
				for(m= qtyCol-1; m >= 0; m--) {
					printf("\e[1;3%um",(rand() % 7+1)); /* set random color */
					printf("%s\n",goLeft[m]);
					printf("\e[1A\e[K");
					/*printf("\e[1A\e[D");*/
					usleep(20000);
					/* if at end of line, go to new line */
					if(m == 0)
						printf("\e[K\n");
				}
			}

		}
		printf("\e[K\e[0m");
		printf("\e[2J"); /* clear rest of screen */
		printf("burp!\n");
		return 0;
	}
}
