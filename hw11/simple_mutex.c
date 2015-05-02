// error: will not work due to not allocating memory manually with *alloc
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void do_one_thing(int *);
void do_another_thing(int *);
void do_wrap_up(int,int);

int r1=0,r2=0,r3=0;
pthread_mutex_t r3_mutex=PTHREAD_MUTEX_INITIALIZER;
int* mat_c;

int main(int argc, char **argv){
	//pthread_t thread1, thread2;

	FILE *fp;
	char str[60];
	int row_a, row_b, col_a, col_b;
	int r1 = 0;
	int r3 = 1;
	int i,j,k;
	int sum;

	int **mat_c= NULL;
	int *mrow;
	int row;

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

	// create array of threads
	pthread_t thrd[row_a];

	int mat_a[row_a][col_a];

	// scan for data
	for(i = 0; i < row_a; i++){
		for(j = 0; j < col_a; j++){
			fscanf(fp,"%d",&mat_a[i][j]);	
		}
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

	int row_c = row_a;
	int col_c = col_b;

	// creating output matrix
	mat_c    = (double **) calloc((size_t) row_c,sizeof(double *));
	mrow = (double *) calloc((size_t) row_c*col_c, sizeof(double))

	// error: unsure how to pass more than one parameter into the parameter ptr for pthreads to work
	int cnt;
	for(cnt = 0; cnt < row_a; cnt++){
		// inputs: ptr to buffer, ptr to thread attribute (NULL=default), ptr to routine thread to execute, ptr to parameter to be pased at which new thread starts
		pthread_create(&thread[cnt], 
				NULL, (void *) multMat, (void *) row_c);
	}

	// wait for thread termination
	for(cnt = 0; cnt < row_a; cnt++){
		pthread_join(thread1, NULL);
	}

	// print mat_c contents
	for(i = 0; i < row_b; i++){
		for(j = 0; j < col_b; j++){
			printf("%d ",mat_c[i][j]);	
		}
		printf("\n");
	}

	fclose(fp);


	return 0;
}

// error did code correct method of obtaining
// mutex acts as a lock protected shared resources 
void multMat(int *val){
	int i,j,k;

	// lock mutex
	pthread_mutex_lock(&r3_mutex);	
	// matrix mult
	for (i = 0; i < row_a; i++) {
		for (j = 0; j < col_b; j++) {
			for (k = 0; k < row_b; k++) {
				sum = sum + mat_a[i][k]*mat_b[k][j];
			}
			mat_c[i][j] = sum;
			sum = 0;
		}
	}

	// unlock mutex
	pthread_mutex_unlock(&r3_mutex);
}

}

