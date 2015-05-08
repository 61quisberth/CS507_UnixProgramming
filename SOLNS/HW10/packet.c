/* packet.c: this program implements both a server and client using datagrams
 *    Author: Charles E. Campbell
 *    Date  : May 05, 2015
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
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <arpa/inet.h>
/*#include <netdb.h>*/
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

if(argc == 3) {	/* packet r port */
	int port;

	if(strcmp(argv[1],"r")) error(XTDIO_ERROR,"Usage: packet [s port message | r port]\n");

	/* get port number */
	sscanf(argv[2],"%d",&port);
	Dprintf((1,"port=%d\n",port));
	Receiver(port);
	}
else if(argc >= 5) {	/* >= 5 arguments: packet s port pktsiz pktqty */
	if(strcmp(argv[1],"s")) error(XTDIO_ERROR,"Usage: packet [s port pktsiz pktqty | r port]\n");
	Sender(argc,argv);
	}
else error(XTDIO_ERROR,"Usage: packet [s port pktsiz pktqty | r port]\n");

Rdbg(("main 0"));
return 0;
}

/* --------------------------------------------------------------------- */
/* Receiver: this function receives a message via a Datagram {{{2 */
void Receiver(int port)
{
int                skt;
struct sockaddr_in from;
socklen_t          fromlen;
int                qty;
char              *pktbuf;
size_t             pktsiz;
size_t             pktqty;
int                cnt;

Edbg(("Receiver(port=%d)",port));

skt= MakeDgramSocket(port+1);

/* fill in from's addr, process information */
memset(&from,0x00,sizeof(struct sockaddr_in)); /* zero out the struct sockaddr_in */
from.sin_family = AF_INET;                     /* family of socket connection     */
from.sin_port   = htons((u_short) port);       /* port number to be used          */
inet_aton("0.0.0.0",&from.sin_addr);           /* use all zeros for current host  */

/* receive message(s) from server
 * recvfrom: MakeDgramSocket provided 3 parts of the 5-tuple: family+protocol, local address, local port
 *           recvfrom() will provide the foreign address, too.
 */
qty= recvfrom(skt,&pktsiz,sizeof(pktsiz),0,(struct sockaddr *) &from,&fromlen);
qty= recvfrom(skt,&pktqty,sizeof(pktqty),0,(struct sockaddr *) &from,&fromlen);
Dprintf((1,"expect to receive %lu packets with %lu bytes each\n",pktqty,pktsiz));
pktbuf= (char *) calloc(pktsiz,sizeof(char));
if(!pktbuf) error(XTDIO_ERROR,"unable to allocate a %lu byte buffer\n",pktsiz);
for(cnt= 1; cnt <= pktqty; ++cnt) {
	qty= recvfrom(skt,pktbuf,pktsiz,0,(struct sockaddr *) &from,&fromlen);
	Dprintf((1,"%4d: rcvd %lu byte packet\n",
	  cnt,
	  qty));
	}

/* close socket */
close(skt);

Rdbg(("Receiver"));
}

/* --------------------------------------------------------------------- */
/* Sender: this function sends a message via a Datagram {{{2 */
void Sender(int argc,char **argv)
{
int                port;
int                skt;
struct sockaddr_in to;
char              *pktbuf;
size_t             pktsiz;
size_t             pktqty;
int                cnt;

Edbg(("Sender(argc=%d,argv<%s %s %s%s>",
  argc,
  argv[1],
  argv[2],
  argv[3],
  (argc > 4)? "..." : ""));

sleep(1);
/* get port number */
sscanf(argv[2],"%d",&port);
Dprintf((1,"port=%d\n",port));

skt= MakeDgramSocket(port);

/* fill in to's addr, process information */
memset(&to,0x00,sizeof(struct sockaddr_in));
to.sin_family = AF_INET;
to.sin_port   = htons((u_short) port+1);

/* set up buffer to be sent */
pktbuf= (char *) calloc(pktsiz,sizeof(char));
memset(pktbuf,'a',pktsiz);

/* send message(s) */
/* sendto: fill in from's addr, process information
 *         MakeDgramSocket provided 3 parts of the 5-tuple: family, protocol, local address
 *         Now providing foreign address, too.
 */
sscanf(argv[3],"%lu",&pktsiz);
sscanf(argv[4],"%lu",&pktqty);
Dprintf((1,"expect to send %lu packets with %lu bytes each\n",pktqty,pktsiz));
sendto(skt,&pktsiz,sizeof(pktsiz),0,(struct sockaddr *) &to,sizeof(struct sockaddr_in));
sendto(skt,&pktqty,sizeof(pktqty),0,(struct sockaddr *) &to,sizeof(struct sockaddr_in));
for(cnt= 1; cnt <= pktqty; ++cnt) {
	Dprintf((1,"%4lu: send %lu bytes\n",cnt,pktsiz));
	sendto(skt,pktbuf,pktsiz,0,(struct sockaddr *) &to,sizeof(struct sockaddr_in));
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
skt= socket(AF_INET,SOCK_DGRAM,0); /* set up a descriptor for a AF_INET datagram socket (1-tuple) */
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
 *	Result: a 3-tuple.
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
