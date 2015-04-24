/* packet.c: this program implements both a server and client using datagrams
 *    Author: Charles E. Campbell, Jr.
 *    Date  : Oct 31, 2008
 *
 *        server: packet s port "message"   sends a message to the client
 *        client: packet c port             prints a message
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
#include <stdio.h>
#include <netinet/in.h>
#include <netdb.h>
#include "xtdio.h"

/* ------------------------------------------------------------------------
 * Definitions: {{{2
 */
#define BUFSIZE	256

/* ------------------------------------------------------------------------
 * Prototypes: {{{2
 */
int main( int, char **);  /* packet.c */
void Receiver(int);       /* packet.c */
void Sender(int,char **); /* packet.c */
int MakeDgramSocket(int); /* packet.c */

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

if(argc < 3) error(XTDIO_ERROR,"Usage: packet [s port message | r port]\n");
if(argc == 3) {	/* packet r port */
	int port;

	if(strcmp(argv[1],"r")) error(XTDIO_ERROR,"Usage: packet [s port message | r port]\n");

	/* get port number */
	sscanf(argv[2],"%d",&port);
	Dprintf((1,"port=%d\n",port));
	Receiver(port);
	}
else {	/* >= 4 arguments: packet s port message */
	if(strcmp(argv[1],"s")) error(XTDIO_ERROR,"Usage: packet [s port message | r port]\n");
	Sender(argc,argv);
	}

Rdbg(("main 0"));
return 0;
}

/* --------------------------------------------------------------------- */
/* Receiver: this function receives a message via a Datagram {{{2 */
void Receiver(int port)
{
char               buf[BUFSIZE];
int                skt;
struct sockaddr_in from;
int                fromlen;
int                qty;

Edbg(("Receiver(port=%d)",port));

skt= MakeDgramSocket(port+1);

/* fill in from's addr, process information */
memset(&from,0x00,sizeof(struct sockaddr_in)); /* zero out the struct sockaddr_in */
from.sin_family = AF_INET;                     /* family of socket connection     */
from.sin_port   = htons((u_short) port);       /* port number to be used          */

/* receive message(s) from server
 * recvfrom: MakeDgramSocket provided 3 parts of the 5-tuple: family, protocol, local address
 *           Now providing foreign address, too.
 */
do {
	buf[0]= '\0';
	qty   = recvfrom(skt,buf,BUFSIZE,0,(struct sockaddr *) &from,&fromlen);
	Dprintf((1,"rcvd<%s> qty=%d  (from port %d)\n",sprt(buf),qty,port));
	printf("<%s>\n",buf);
	sleep(1);
	if(qty == 5 && !strcmp(buf,"quit")) break;
	} while(qty >= 0);

/* close socket */
close(skt);

Rdbg(("Receiver"));
}

/* --------------------------------------------------------------------- */
/* Sender: this function sends a message via a Datagram {{{2 */
void Sender(int argc,char **argv)
{
char    buf[BUFSIZE];
int     port;
int     skt;
struct sockaddr_in to;

Edbg(("Sender(argc=%d,argv<%s %s %s%s>",
  argc,
  argv[1],
  argv[2],
  argv[3],
  (argc > 4)? "..." : ""));

/* get port number */
sscanf(argv[2],"%d",&port);
Dprintf((1,"port=%d\n",port));

skt= MakeDgramSocket(port);

/* fill in to's addr, process information */
memset(&to,0x00,sizeof(struct sockaddr_in));
to.sin_family = AF_INET;
to.sin_port   = htons((u_short) port+1);

/* send message(s) */
/* sendto: fill in from's addr, process information
 *         MakeDgramSocket provided 3 parts of the 5-tuple: family, protocol, local address
 *         Now providing foreign address, too.
 */
for(argc-= 3, argv+= 3; argc > 0; --argc, ++argv) {
	Dprintf((1,"send<%s> %d bytes (to port %d)\n",sprt(*argv),strlen(*argv)+1,port+1));
	sendto(skt,*argv,strlen(*argv)+1,0,(struct sockaddr *) &to,sizeof(struct sockaddr_in));
	}

close(skt);

Rdbg(("Sender"));
}

/* --------------------------------------------------------------------- */
/* MakeDgramSocket: takes a port and opens a datagram (connectionless) socket {{{2 */
int MakeDgramSocket(int port)
{
int                skt;
struct sockaddr_in sin;              /* Internet socket address structure */

Edbg(("MakeDgramSocket(port=%d)",port));

/* int socket(int domain, int type, int protocol)
 * Provides a socket descriptor with a family and protocol, but no address
 */
Dprintf((1,"socket'ing...\n"));
skt= socket(AF_INET,SOCK_DGRAM,0); /* set up a descriptor for a AF_INET datagram socket */
if(skt < 0) {
	perror("MakeDgramSocket socket() failure: ");
	error(XTDIO_ERROR,"unable to open a socket!\n");
	}
Dprintf((1,"socket() succeeded\n"));

/* fill in localaddr, process information */
memset(&sin,0x00,sizeof(struct sockaddr_in)); /* zero out sockaddr_in */
sin.sin_family      = AF_INET;                /* set  up family       */
sin.sin_port        = htons((u_short) port);  /* set up port          */

/*	bind socket data structure to this socket
 *	Provides the socket with local address information.
 */
Dprintf((1,"binding...\n"));
if(bind(skt,(struct sockaddr *) &sin,sizeof(sin))) {
	perror("MakeDgramSocket bind() failure: ");
	error(XTDIO_ERROR,"MakeDgramSocket bind() failure\n");
	}
Dprintf((1,"bind() succeeded\n"));

Rdbg(("MakeDgramSocket %d",skt));
return skt;
}

/* ===================================================================== */
/* Modelines: {{{1
 *  vim: fdm=marker
 */
