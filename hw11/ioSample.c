#include <stdio.h>

int main()
{
	FILE *fp;
	char str[60];
	int row_a, row_b, col_a, col_b;
	int i,j,k;
	int sum;

	/* opening file for reading */
	fp = fopen("input" , "r");
	if(fp == NULL) 
	{
		perror("Error opening file");
		return(-1);
	}
	// scan for first matrix size
	fscanf(fp, "%d",&row_a);
	fscanf(fp, "%d",&col_a);
	printf("row: %d col: %d\n",row_a,col_a);
	int mat_a[row_a][col_a];

	// scan for data
	for(i = 0; i < row_a; i++){
		for(j = 0; j < col_a; j++){
			fscanf(fp,"%d",&mat_a[i][j]);	
		}
	}

	// print data
	for(i = 0; i < row_a; i++){
		for(j = 0; j < col_a; j++){
			printf("%d ",mat_a[i][j]);	
		}
		printf("\n");
	}

	// scan for second matrix size
	fscanf(fp, "%d",&row_b);
	fscanf(fp, "%d",&col_b);
	printf("row: %d col: %d\n",row_b,col_b);
	int mat_b[row_b][col_b];

	// scan for data
	for(i = 0; i < row_b; i++){
		for(j = 0; j < col_b; j++){
			fscanf(fp,"%d",&mat_b[i][j]);	
		}
	}

	// print for data
	for(i = 0; i < row_b; i++){
		for(j = 0; j < col_b; j++){
			printf("%d ",mat_b[i][j]);	
		}
		printf("\n");
	}

	int row_c = row_a;
	int col_c = col_b;
	// creating output matrix
	int mat_c[row_c][col_c];
	for (i = 0; i < row_a; i++) {
		for (j = 0; j < col_b; j++) {
			for (k = 0; k < row_b; k++) {
				sum = sum + mat_a[i][k]*mat_b[k][j];
			}
			mat_c[i][j] = sum;
			sum = 0;
		}
	}
	
	printf("result\n");

	// print mat_c contents
	for(i = 0; i < row_b; i++){
		for(j = 0; j < col_b; j++){
			printf("%d ",mat_c[i][j]);	
		}
		printf("\n");
	}

	fclose(fp);

	return(0);
}


