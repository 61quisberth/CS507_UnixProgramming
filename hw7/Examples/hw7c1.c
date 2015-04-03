#include <stdio.h>

int main ( int argc, char *argv[] )
{
	if ( argc != 2 ) /* argc should be 2 for correct execution */
	{
		/* We print argv[0] assuming it is the program name */
		printf( "usage: %s number\n", argv[0] );
	}
	else{
		printf("Hello input value: %c\n",*argv[1]);
	}
	return 0;
}


