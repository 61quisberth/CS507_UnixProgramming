/* prototypes generated automatically by Dr. Chip's flist program
 * Wed Jul 9 13:31:30 EDT 2008
 */
 
char *cprt(const char);                                                                                                                /* cprt.c          */
void prt_traceback(void);                                                                                                              /* dbg.c           */
void edbg(char *,...);                                                                                                                 /* dbg.c           */
void rdbg(char *,...);                                                                                                                 /* dbg.c           */
int dprintf(int,char *,...);                                                                                                           /* dbg.c           */
void dbgchkswitch(int);                                                                                                                /* dbg.c           */
int dbgswitch(int);                                                                                                                    /* dbg.c           */
void initdbg( int *, char **);                                                                                                         /* dbg.c           */
void initdbg2(char *);                                                                                                                 /* dbg.c           */
void initdbg3(char *);                                                                                                                 /* dbg.c           */
void prt_dbgstr(char *,...);                                                                                                           /* dbg.c           */
void dsetjmp(void *);                                                                                                                  /* dbg.c           */
void dlongjmp(void *,char *,...);                                                                                                      /* dbg.c           */
int dbgtst(int);                                                                                                                       /* dbg.c           */
void prt_debugstate(char *,...);                                                                                                       /* dbg.c           */
FILE *dbgfpget(void);                                                                                                                  /* dbg.c           */
void error( int, char *, ...);                                                                                                         /* error.c         */
int main( int, char **);                                                                                                               /* example.c       */
void f1(int);                                                                                                                          /* example.c       */
void f2(void);                                                                                                                         /* example.c       */
int f3(int);                                                                                                                           /* example.c       */
FILE *fopenv(char *,char *,char *);                                                                                                    /* fopenv.c        */
void outofmem(void *,char *,...);                                                                                                      /* outofmem.c      */
void rdcolor(void);                                                                                                                    /* rdcolor.c       */
void rdcputs(char *,FILE *);                                                                                                           /* rdcolor.c       */
void setup_screendata(char *);                                                                                                         /* rdcolor.c       */
char *sprt(const void *);                                                                                                              /* sprt.c          */
char *srmtrblk(char *);                                                                                                                /* srmtrblk.c      */
char *stpblk(char *);                                                                                                                  /* stpblk.c        */
char *stpbrk(char *,char *);                                                                                                           /* stpbrk.c        */
char *stpnxt( char *,  char *);                                                                                                        /* stpnxt.c        */
 
