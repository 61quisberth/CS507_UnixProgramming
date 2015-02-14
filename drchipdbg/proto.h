/* prototypes generated automatically by Dr. Chip's flist program
 * Fri May 21 09:42:56 EDT 1999
 */
#ifdef __PROTOTYPE__
 
void prt_traceback(void);                                                                                           /* dbg.c           */
void edbg(char *,...);                                                                                              /* dbg.c           */
void rdbg(char *,...);                                                                                              /* dbg.c           */
int dprintf(int,char *,...);                                                                                        /* dbg.c           */
int dbgswitch(int);                                                                                                 /* dbg.c           */
void initdbg(int *,char **);                                                                                        /* dbg.c           */
void initdbg2(char *);                                                                                              /* dbg.c           */
void initdbg3(char *);                                                                                              /* dbg.c           */
void prt_dbgstr(char *,...);                                                                                        /* dbg.c           */
void dsetjmp(void *);                                                                                               /* dbg.c           */
void dlongjmp(void *,char *,...);                                                                                   /* dbg.c           */
int dbgtst(int);                                                                                                    /* dbg.c           */
void prt_debugstate(char *,...);                                                                                    /* dbg.c           */
FILE *dbgfpget(void);                                                                                               /* dbg.c           */
int main( int, char **);                                                                                            /* example.c       */
void f1(int);                                                                                                       /* example.c       */
void f2(void);                                                                                                      /* example.c       */
int f3(int);                                                                                                        /* example.c       */
 
#else	/* __PROTOTYPE__ */
 
extern void prt_traceback();                                                                                        /* dbg.c           */
extern void edbg();                                                                                                 /* dbg.c           */
extern void rdbg();                                                                                                 /* dbg.c           */
extern int dprintf();                                                                                               /* dbg.c           */
extern int dbgswitch();                                                                                             /* dbg.c           */
extern void initdbg();                                                                                              /* dbg.c           */
extern void initdbg2();                                                                                             /* dbg.c           */
extern void initdbg3();                                                                                             /* dbg.c           */
extern void prt_dbgstr();                                                                                           /* dbg.c           */
extern void dsetjmp();                                                                                              /* dbg.c           */
extern void dlongjmp();                                                                                             /* dbg.c           */
extern int dbgtst();                                                                                                /* dbg.c           */
extern void prt_debugstate();                                                                                       /* dbg.c           */
extern FILE *dbgfpget();                                                                                            /* dbg.c           */
extern int main();                                                                                                  /* example.c       */
extern void f1();                                                                                                   /* example.c       */
extern void f2();                                                                                                   /* example.c       */
extern int f3();                                                                                                    /* example.c       */
 
#endif	/* __PROTOTYPE__ */
