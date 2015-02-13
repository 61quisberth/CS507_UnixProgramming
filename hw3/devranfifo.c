#include <stdio.h>

int main(int argc ,char **argv)
{
	FILE *fp;
	int cap = 0; /* base case in the event no input from sscanf */
	int rej = 0;
	int acc = 0;
	int qty;
	int c;

	sscanf(argv[1],"%d",&qty);
	char str[qty];


	fp = fopen("/dev/random","r");

	/* if file is invalid throw error */
	if(fp == NULL) 
	{
		perror("Error in opening file");
		return(-1);
	}

	do{
		c = fgetc(fp);
		if(c > 32 && c<126){
			str[cap] = c;
			printf("%c ",str[cap]);		
			acc++;
			cap++;
		}
		else
			rej++;
	}
	while(cap < qty);	

	printf("\nnum rej: %d\nnum acc: %d\n",rej,acc);
	
	fclose(fp);
	
	return 0;
}
