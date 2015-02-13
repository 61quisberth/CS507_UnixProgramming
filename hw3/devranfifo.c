/* Jason Quisberth hw 3 CS 507 CUA 
   Using FIFO's to obtain quantity, print sequence of ASCII characters from a certain range from /dev/random
*/
#include <stdio.h>

int main(int argc ,char **argv)
{
	FILE *fp;
	FILE *fifoFP;
	int cap = 0; /* base case in the event no input from fscanf */
	int rej = 0;
	int acc = 0;
	int qty;
	int c;

	fifoFP = fopen("./devranfifo.in","r"); /* creates ptr to a FIFO "file"  */	
	fscanf(fifoFP,"%d",&qty); /* scan and take input from file pointer */
	char str[qty];


	fp = fopen("/dev/random","r");

	/* if file is invalid throw error */
	if(fp == NULL) 
	{
		perror("Error in opening file");
		return(-1);
	}
	
	/* file is valid: count valid and invalid characters in range until valid count is equal to value from FIFO */
	do{
		c = fgetc(fp);
		if(c >= 32 && c <= 126){
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
	fclose(fifoFP);

	return 0;
}
