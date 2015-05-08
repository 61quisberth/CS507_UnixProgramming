/* ex2.c: this program illustrates a very simple core dumping problem:
 *        de-referencing a null pointer
 */

/* =====================================================================
 * Header Section: {{{1
 */

/* ---------------------------------------------------------------------
 * Includes: {{{2
 */
#include <stdio.h>

/* =====================================================================
 * Functions: {{{1
 */

/* --------------------------------------------------------------------- */
/* main: {{{2 */
int main(
  int    argc,
  char **argv)
{
int *pi= NULL;    /* initialized a pointer to int to null */
printf("%d",*pi); /* whoops                               */
return 0;
}

/* ===================================================================== */
/* Modelines: {{{1
 *  vim: fdm=marker
 */
