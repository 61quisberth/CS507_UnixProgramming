/* ex3.c: this program illustrates calloc/free and what happens when you access a free'd pointer
 *   Try this three ways:
 *     * as is  (cc -g ex3.c -o ex3)
 *     * change  #if 0  to   #if 1
 *     * compile with -lefence and run with #if 0
 *       (cc -g ex3.c -lefence -o ex3)
 */

/* =====================================================================
 * Header Section: {{{1
 */

/* ---------------------------------------------------------------------
 * Includes: {{{2
 */
#include <stdio.h>
#include <stdlib.h>

/* ========================================================================
 * Functions: {{{1
 */

/* --------------------------------------------------------------------- */
/* main: {{{2 */
int main(
  int    argc,
  char **argv)
{
int *pi= NULL;

/* allocate and initialize pi */
pi   = (int *) calloc((size_t) 5, sizeof(int));
pi[0]= 1;

/* free pi */
free((char *) pi);
#if 0
/* this example illustrates one very good reason
 * why you should *always* set a pointer to null
 * after you've freed it
 */
pi= NULL;
#endif

printf("p[0]=%d\n",pi[0]);
return 0;
}

/* ===================================================================== */
/* Modelines: {{{1
 *  vim: fdm=marker fenc=utf-8 ts=4
 */

