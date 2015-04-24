/* connect.c: this program implements both a server and client using
 *        basic socket IPC -- connection oriented sockets
 *
 *        server: connect s port "message"   sends a message to the client
 *        client: connect c port             prints a message
 */

/* =====================================================================
 * Header Section: {{{1
 */

/* ---------------------------------------------------------------------
 * Includes: {{{2
 */
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <sys/uio.h>
#include <unistd.h>
#include "xtdio.h"

/* ------------------------------------------------------------------------
 * Definitions: {{{2
 */
#define BUFSIZE	256

/* ------------------------------------------------------------------------
 * Prototypes: {{{2
 */
int main( int, char **);  /* connection.c */
void Client(int);         /* connection.c */
void Server(int,char **); /* connection.c */

/* ========================================================================
 * Functions: {{{1
 */

/* --------------------------------------------------------------------- */
/* main: {{{2 */
int main(
  int    argc,
  char **argv)
{
int port;

Initdbg((&argc,argv));
rdcolor();

if(argc < 3) error(XTDIO_ERROR,"Usage: connection [s port message | c port]\n");
if(argc == 3) {
	if(strcmp(argv[1],"c")) error(XTDIO_ERROR,"Usage: connection [s port message | c port]\n");
	sscanf(argv[2],"%d",&port);
	Client(port);
	}
else {	/* >= 4 arguments */
	if(strcmp(argv[1],"s")) error(XTDIO_ERROR,"Usage: connection [s port message | c port]\n");
	Server(argc,argv);
	}

Rdbg(("main 0"));
return 0;
}

/* --------------------------------------------------------------------- */
/* Client: this function implements a client using the connection-oriented {{{2
 *         sockets IPC
 */
void Client(int port)
{
char                buf[BUFSIZE];
char                hostname[BUFSIZE];
char               *b;
int                 qtyread;
int                 skt;
struct hostent     *hostentptr        = NULL;
struct sockaddr_in  sin;                      /* InterNet socket address structure */

Edbg(("Client(port=%d)",port));

/* get current hostname */
Dprintf((1,"getting current hostname...\n"));
// TODO: observe /etc/hosts
// struct hostent *lh = gethostbyname("localhost");
// if(gethostname(hostname,BUFSIZE) == -1) { TODO; edit this code to use gethostbyname and localhost
if(gethostname(hostname,BUFSIZE) == -1) {
	perror("Client gethostname() failure: ");
	error(XTDIO_ERROR,"unable to open a socket!\n");
	}
printf("%s\n",hostname);
Dprintf((1,"gethostname() succeeded: hostname<%s>\n",sprt(hostname)));

/* gethostbyname() returns a pointer to
 * struct hostent {
 *                char  *h_name;      // official name of host
 *                char **h_aliases;   // alias list
 *                int    h_addrtype;  // host address type
 *                int    h_length;    // length of address
 *                char **h_addr_list; // list of addresses from name server
 *                };
 */
hostentptr= gethostbyname(hostname);
if(!hostentptr) {
	perror("Client gethostbyname() failure: ");
	error(XTDIO_ERROR,"Client gethostbyname(%s) failure\n",sprt(hostname));
	}
Dprintf((1,"gethostbyname succeeded\n"));

/* initialize sockaddr_in structure */
memset(&sin,0x00,sizeof(struct sockaddr_in));              /* zero out struct sockaddr_in structure */
sin.sin_family = AF_INET;                                  /* set family to AF_INET -- internet     */
sin.sin_addr   = *((struct in_addr *) hostentptr->h_addr); /* gethostbyname() provided this         */
sin.sin_port   = htons((u_short) port);                    /* include port number                   */

/* poll, attempting to open socket to server once a second */
while(1) {
	Dprintf((1,"socket'ing...\n"));
	skt= socket(AF_INET,SOCK_STREAM,0); /* int socket(int domain, int type, int protocol) */
	if(skt < 0) {
		perror("Client socket() failure: ");
		error(XTDIO_ERROR,"unable to open a socket!\n");
		}
	Dprintf((1,"socket() succeeded\n"));
	Dprintf((1,"connecting...\n"));
	if(connect(skt,(struct sockaddr *) &sin,sizeof(sin)) < 0) sleep(1);
	else                                                      break;
	close(skt);
	}
Dprintf((1,"connect() succeeded\n"));

/* receive message(s) from server */
while(1) {
	/* note: this is a quick&dirty but bad method.  The
	 * socket is a *stream*, not a packet virtual device.
	 * Consequently only part of a string may arrive in
	 * one read.
	 */
	if((qtyread= read(skt,buf,BUFSIZE)) <= 0) break;
	Dprintf((1,"rcvd<%s> qtyread=%d\n",sprt(buf),qtyread));
	printf("read: %d bytes\n",qtyread);
	for(b= buf; b - buf < qtyread; b+= strlen(b)+1) {
		printf("rcvd<%s> %ld bytes\n",b,strlen(b)+1);
		}
	}

/* close socket */
close(skt);

Rdbg(("Client"));
}

/* --------------------------------------------------------------------- */
/* Server: this function implements a server using the connection-oriented {{{2
 *         sockets IPC
 */
void Server(int argc,char **argv)
{
int                port;
int                skt;
int                srvr;
int                status       = 1;
unsigned           addrlen;
struct sockaddr_in sin;              /* InterNet socket address structure */

Edbg(("Server(argc=%d,argv<%s %s %s%s>)",
  argc,
  argv[1],
  argv[2],
  argv[3],
  (argc > 4)? "..." : ""));

/* create socket: specify family, domain, protocol */
srvr= socket(AF_INET,SOCK_STREAM,0);
if(srvr < 0) {
	perror("Server socket() failure: ");
	error(XTDIO_ERROR,"Server socket() failure\n");
	}
Dprintf((1,"server socket created\n"));

/* allow re-use of socket port */
if(setsockopt(srvr,SOL_SOCKET,SO_REUSEADDR,(char *) &status,sizeof(status)) < 0) {
	perror("Server setsockopt() failure: ");
	error(XTDIO_ERROR,"Server setsockopt() failure\n");
	}

/* fill in localaddr, process information */
sscanf(argv[2],"%d",&port);
Dprintf((1,"port=%d\n",port));
memset(&sin,0x00,sizeof(struct sockaddr_in));
sin.sin_family      = AF_INET;
sin.sin_addr.s_addr = htonl(INADDR_ANY);
sin.sin_port        = htons((u_short) port);

/*	bind socket data structure to this socket     */
Dprintf((1,"binding...\n"));
if(bind(srvr,(struct sockaddr *) &sin,sizeof(sin))) {
	perror("Server bind() failure: ");
	error(XTDIO_ERROR,"Server bind() failure\n");
	}
Dprintf((1,"bind() succeeded\n"));

/* specify how many clients may queue for connection */
Dprintf((1,"listening...\n"));
listen(srvr,5);
Dprintf((1,"listen() succeeded\n"));

/* attempt to accept a client (finish making the 5-tuple connection) */
addrlen= sizeof(sin);
Dprintf((1,"accepting...\n"));
skt    = accept(srvr,(struct sockaddr *) &sin,&addrlen);
if(skt == -1) {
	perror("Server accept() failure: ");
	error(XTDIO_ERROR,"Server accept() failure\n");
	}
Dprintf((1,"accept() succeeded\n"));

/* send message(s) */
for(argc-= 3, argv+= 3; argc > 0; --argc, ++argv) {
	Dprintf((1,"send<%s>\n",sprt(*argv)));
	/* as with the Client(), this is a bad method
	 * for transmitting data, because a write()
	 * may not completely write the string.
	 */
	write(skt,*argv,strlen(*argv)+1);
	}

/* close accept-socket and server socket */
close(skt);
close(srvr);

Rdbg(("Server"));
}

/* ===================================================================== */
/* Modelines: {{{1
 *  vim: fdm=marker
 */
