/* ex1.c: a small use of pointers (core dump due to dereferencing a null pointer) */
#include <stdio.h>

/* --------------------------------------------------------------------- */
/* main: {{{1 */
int main()
{
int *pi= NULL;	/* null pointer */

*pi= 2;			/* attempt to access value stored by null pointer */
}

/* ===================================================================== */
/* Modelines: {{{1
 *  vim: fdm=marker fenc=utf-8 ts=4
 */
