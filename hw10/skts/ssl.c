/* ssl.c: this program implements both a server and client using
 *        the Simple Sockets Library.
 *        Author: Charles E. Campbell, Jr.
 *        Date  : Oct 31, 2008
 *
 *        server: ssl s srvrname "message"   sends a message to the client
 *        client: ssl c srvrname             prints a message
 */

/* =====================================================================
 * Header Section: {{{1
 */

/* ---------------------------------------------------------------------
 * Includes: {{{2
 */
#include <stdio.h>
#include <unistd.h>
#include "xtdio.h"
#include "sockets.h"

/* ------------------------------------------------------------------------
 * Definitions: {{{2
 */
#define BUFSIZE	256

/* ------------------------------------------------------------------------
 * Prototypes: {{{2
 */
int main( int, char **);  /* ssl.c */
void Client(char *);      /* ssl.c */
void Server(int,char **); /* ssl.c */

/* =====================================================================
 * Functions: {{{1
 */

/* --------------------------------------------------------------------- */
/* main: {{{2 */
int main(
  int    argc,
  char **argv)
{
Initdbg((&argc,argv));
rdcolor();

if(argc < 3) error(XTDIO_ERROR,"Usage: ssl [s srvrname message | c srvrname]\n");
if(argc == 3) {
	if(strcmp(argv[1],"c")) error(XTDIO_ERROR,"Usage: ssl [s srvrname message | c srvrname]\n");
	Client(argv[2]);
	}
else {	/* >= 4 arguments */
	if(strcmp(argv[1],"s")) error(XTDIO_ERROR,"Usage: ssl [s srvrname message | c srvrname]\n");
	Server(argc,argv);
	}

Rdbg(("main 0"));
return 0;
}

/* --------------------------------------------------------------------- */
/* Client: this function implements a client using the Simple Sockets Library {{{2 */
void Client(char *srvrname)
{
char    buf[BUFSIZE];
Socket *skt= NULL;

Edbg(("Client(srvrname<%s>)",sprt(srvrname)));

/* poll to open socket to client once a second */
while(1) {
	skt= Sopen(srvrname,"c");
	if(skt) break;
	sleep(1);
	}

/* receive message(s) from server */
while(Sgets(buf,BUFSIZE,skt)) {
	Dprintf((1,"rcvd<%s>\n",sprt(buf)));
	printf("rcvd<%s>\n",sprt(buf));
	}

/* close socket */
Sclose(skt);

Rdbg(("Client"));
}

/* --------------------------------------------------------------------- */
/* Server: this function implements a server using the Simple Sockets Library {{{2 */
void Server(int argc,char **argv)
{
char    buf[BUFSIZE];
Socket *skt = NULL;
Socket *srvr= NULL;

Edbg(("Server(argc=%d,argv<%s %s %s%s>)",
  argc,
  argv[1],
  argv[2],
  argv[3],
  (argc > 4)? "..." : ""));

/* open the server */
srvr= Sopen(argv[2],"S");
if(!srvr) error(XTDIO_ERROR,"Server: unable to open <%s> server\n",argv[2]);

/* attempt to accept a client */
skt= Saccept(srvr);
if(!skt) error(XTDIO_ERROR,"Server: socket error with srvr\n");

/* send message(s) */
for(argc-= 3, argv+= 3; argc > 0; --argc, ++argv) {
	Dprintf((1,"send<%s>\n",sprt(*argv)));
	Sputs(*argv,skt);
	}

/* close accept-socket and server socket */
Sclose(skt);
Sclose(srvr);

Rdbg(("Server"));
}

/* ===================================================================== */
/* Modelines: {{{1
 *  vim: fdm=marker
 */
