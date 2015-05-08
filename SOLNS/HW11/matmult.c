/* matmult.c: this program multiplies two matrices together using pthreads
 *   Author: Charles E. Campbell
 *   Date:   May 05, 2015
 */

/* =====================================================================
 * Header Section: {{{1
 */

/* ---------------------------------------------------------------------
 * Includes: {{{2
 */
#include <stdio.h>
#include <pthread.h>
#include "xtdio.h"

/* ------------------------------------------------------------------------
 * Definitions: {{{2
 */

/* ------------------------------------------------------------------------
 * Typedefs: {{{2
 */
typedef struct DMatrix_str DMatrix;

/* ------------------------------------------------------------------------
 * Local Data Structures: {{{2
 */
struct DMatrix_str {
	double **mat;
	int      rows;
	int      cols;
	};

/* ------------------------------------------------------------------------
 * Global Data: {{{2
 */
DMatrix *A = NULL;
DMatrix *B = NULL;
DMatrix *C = NULL;

/* ------------------------------------------------------------------------
 * Explanation: {{{2
 */

/* ------------------------------------------------------------------------
 * Prototypes: {{{2
 */
int main( int, char **);             /* matmult.c */
void RowMult(int *);                 /* matmult.c */
DMatrix *Dmatmake( int, int);        /* matmult.c */
DMatrix *Dmatget(FILE *,DMatrix **); /* matmult.c */

/* ========================================================================
 * Functions: {{{1
 */

/* --------------------------------------------------------------------- */
/* main: {{{2 */
int main(
  int    argc,
  char **argv)
{
FILE      *fp    = NULL;
int        i;
int        j;
int        r;
int        retval;
int       *pretval;
int       *row   = NULL;
pthread_t *thrds = NULL;

Initdbg((&argc,argv));
rdcolor();

if(argc <= 1) error(XTDIO_ERROR,"***usage*** matmult filename\n");

fp= fopen(argv[1],"r");
if(!fp) error(XTDIO_ERROR,"unable to open file<%s> for reading\n",argv[1]);

A = Dmatget(fp,&A);  /* get the A matrix */
B = Dmatget(fp,&B);  /* get the B matrix */

/* sanity check */
if(A->cols != B->rows) error(XTDIO_ERROR,"A*B doesn't exist, A->cols=%d and B->rows=%d (must be the same)\n",A->cols,B->rows);

C     = Dmatmake(A->rows,B->cols); /* set up the C matrix */
thrds = (pthread_t *) calloc((size_t) C->rows, sizeof(pthread_t));
row   = (int *) calloc((size_t) C->rows,sizeof(int));

/* set up an integer vector so that each thread may access its own private row specifier */
for(r= 0; r < C->rows; ++r) row[r]= r;

/* fire off a thread, one per row of C */
for(r= 0; r < C->rows; ++r) {
	pthread_create(&thrds[r],NULL,(void *)RowMult,(void *) &row[r]);
	}

/* wait for all threads to finish */
pretval= &retval;
for(r= 0; r < C->rows; ++r) {
	pthread_join(thrds[r],(void **)&pretval);
	}

/* display the output */
printf("C:\n");
for(i= 0; i < C->rows; ++i) {
	for(j= 0; j < C->cols; ++j) {
		printf("%12.5lf",C->mat[i][j]);
		}
	printf("\n");
	}

Rdbg(("main 0"));
return 0;
}

/* --------------------------------------------------------------------- */
/* RowMult: this function forms row *pr of C {{{2 */
void RowMult(int *pr)
{
int    r;
int    j;
int    k;
double sum;

Edbg(("RowMult(%spr=%d)",
  pr? ""  : "null ",
  pr? *pr : 0));

/* form row r of C from A and B */
r= *pr;
for(j= 0; j < B->cols; ++j) {
	for(k= 0, sum= 0.; k < B->rows; ++k) sum+= A->mat[r][k]*B->mat[k][j];
	C->mat[r][j]= sum;
	}

pthread_exit(0);

Rdbg(("RowMult"));
return;
}

/* --------------------------------------------------------------------- */
/* Dmatmake: this function forms a rows x cols matrix {{{2 */
DMatrix *Dmatmake(
  int   rows,
  int   cols)
{
DMatrix *M = NULL;
int      r;
double  *mrow;

Edbg(("Dmatmake(rows=%d,cols=%d)",
  rows,
  cols));

M      = (DMatrix *) malloc(sizeof(DMatrix));
M->mat = (double **) calloc((size_t) rows,sizeof(double *));
mrow   = (double  *) calloc((size_t) rows*cols,sizeof(double));

M->rows = rows;
M->cols = cols;
for(r= 0; r < rows; ++r, mrow+= cols) M->mat[r]= mrow;

Rdbg(("Dmatmake %s : %dx%d",
  M? "M" : "null",
  M->rows,
  M->cols));
return M;
}

/* --------------------------------------------------------------------- */
/* Dmatget: this function gets a DMatrix from the specified file {{{2 */
DMatrix *Dmatget(FILE *fp,DMatrix **pM)
{
int      rows;
int      cols;
int      r;
int      c;
DMatrix *M;

Edbg(("Dmatget(%sfp,%spM)",
  fp? "" : "null ",
  pM? "" : "null "));

fscanf(fp,"%d",&rows);
fscanf(fp,"%d",&cols);
Dprintf((1,"getting a %dx%d DMatrix\n",rows,cols));

M= Dmatmake(rows,cols);
if(pM) *pM= M;

for(r= 0; r < rows; ++r) {
	for(c= 0; c < cols; ++c) {
		fscanf(fp,"%lf",&M->mat[r][c]);
		Dprintf((2,"M->mat[%2d][%2d]=%12.5lf\n",r,c,M->mat[r][c]));
		}
	}

Rdbg(("Dmatget %sM",M? "" : "null "));
return M;
}

/* ===================================================================== */
/* Modelines: {{{1
 *  vim: fdm=marker fenc=utf-8 ts=4
 */
