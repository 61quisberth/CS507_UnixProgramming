// use of type modifiers

#include <stdio.h>
int main(int argc, char* argv[]){
	int x = 2;
	int *px = &x;
	*px = 8;
	printf("%d\n",x);
	
	struct plant_str{
		int stem;
		char id;	
	};

	int b[5] = {2,1,3,4,7};
	printf("%d\n",b[1]);

	return 0;
}
