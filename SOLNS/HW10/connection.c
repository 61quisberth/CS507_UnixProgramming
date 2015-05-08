/* connect.c: this program implements both a server and client using
 *        basic socket IPC -- connection oriented sockets
 *
 *        server: connect s port "message"   sends a message to the client
 *        client: connect c port             prints a message
 *
 * If Q2 is #define'd (compile with -DQ2) then this program sends all message strings from server to client
 * Else this program will expect the server command to be
 *        	connect s port pktsiz pktqty
 *        and will sent pktqty packets with pktsiz bytes filled with "a" characters from server to client
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
#include <errno.h>
#include "xtdio.h"

/* ------------------------------------------------------------------------
 * Definitions: {{{2
 */
#define BUFSIZE	256
#if 0
# define Q2		/* if you want to see the rcvd messages */
#endif

/* ------------------------------------------------------------------------
 * Prototypes: {{{2
 */
int main( int, char **);                   /* connection.c */
void Client(int);                          /* connection.c */
void Server( int, char **);                /* connection.c */
void Write( int, void *, size_t);          /* connection.c */
void Read( int, size_t, void *, size_t *); /* connection.c */

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
char                hostname[BUFSIZE];
size_t              qtyread;
int                 skt;
struct hostent     *hostentptr        = NULL;
struct sockaddr_in  sin;                      /* InterNet socket address structure */
static char        *buf               = NULL;
static size_t       buflen            = 0L;
#if defined(DEBUG) && !defined(Q2)
size_t              cnt= 0l;
#endif

Edbg(("Client(port=%d)",port));

/* get current hostname */
#if USELOCALHOST
strcpy(hostname,"localhost");
Dprintf((1,"using hostname<%s>\n",sprt(hostname)));
#else
Dprintf((1,"getting current hostname...\n"));
if(gethostname(hostname,BUFSIZE) == -1) {
	perror("Client gethostname() failure: ");
	error(XTDIO_ERROR,"unable to open a socket!\n");
	}
Dprintf((1,"gethostname() succeeded: hostname<%s>\n",sprt(hostname)));
#endif

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
memset(&sin,0x00,sizeof(struct sockaddr_in));
sin.sin_family= AF_INET;
sin.sin_addr= * ((struct in_addr *) hostentptr->h_addr);
sin.sin_port= htons((u_short) port);

/* poll to open socket to client once a second */
while(1) {
	/* int socket(int domain, int type, int protocol) */
	Dprintf((1,"socket'ing...\n"));
	skt= socket(AF_INET,SOCK_STREAM,0);
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
	Read(skt,sizeof(qtyread),&qtyread,NULL);
	qtyread= ntohl(qtyread);
	if(qtyread == 0l) break;
	Read(skt,qtyread,&buf,&buflen);
#ifdef Q2
	printf("rcvd %3lu bytes: %s\n",qtyread,buf);
#elif defined(DEBUG)
	Dprintf((1,"%4lu: rcvd %4lu bytes\n",++cnt,qtyread));
#endif
	}

/* close socket */
Dprintf((1,"closing socket skt#%d\n",skt));
close(skt);

Rdbg(("Client"));
}

/* --------------------------------------------------------------------- */
/* Server: this function implements a server using the connection-oriented {{{2
 *         sockets IPC
 */
void Server(
  int    argc,
  char **argv)
{
int                port;
int                skt;
int                srvr;
int                status  = 1;
size_t             cnt;
unsigned           addrlen;
struct sockaddr_in sin;     /* InterNet socket address structure   */
#ifndef Q2
size_t             npktsiz; /* network-style pktsiz                */
size_t             qtymsg;  /* quantity of messages to be sent     */
size_t             pktsiz;  /* size of messages to be sent         */
char              *pktbuf;  /* buffer to hold pktsiz bytes of "a"s */
#endif

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
#ifdef Q2
for(argc-= 3, argv+= 3; argc > 0; --argc, ++argv) {
	Dprintf((1,"send<%s>\n",sprt(*argv)));
	cnt= htonl(strlen(*argv) + 1);
	Dprintf((1,"write qty of bytes about to be sent (%lu)\n",strlen(*argv)+1));
	Write(skt,&cnt,sizeof(cnt));      /* write qty of bytes about to be sent */
	Dprintf((1,"write the string<%s>\n",sprt(*argv)));
	Write(skt,*argv,strlen(*argv)+1); /* write the string                    */
	}
#else
if(argc >= 5) {
	sscanf(argv[3],"%lu",&pktsiz);
	sscanf(argv[4],"%lu",&qtymsg);
	pktbuf = (char *) calloc(pktsiz,sizeof(char));
	if(!pktbuf) error(XTDIO_ERROR,"unable to allocate %lu byte pktbuf buffer\n",pktsiz);
	npktsiz= htonl(pktsiz);
	memset(pktbuf,'a',pktsiz);
	for(cnt= 0; cnt < qtymsg; ++cnt) {
		Write(skt,&npktsiz,sizeof(npktsiz));
		pktsiz = ntohl(npktsiz);
		Write(skt,pktbuf,pktsiz);
		Dprintf((1,"%4lu: wrote %4lu bytes\n",cnt+1,pktsiz));
		}
	}
else error(XTDIO_ERROR,"***usage*** connection s port msglen qtymsg\n");
#endif

/* indicate end-of-messages with a zero count */
Dprintf((1,"write end-of-message (zero count)\n"));
cnt= 0;
Write(skt,&cnt,sizeof(cnt));

/* close accept-socket and server socket */
Dprintf((1,"closing accept-socket skt#%d\n",skt));
close(skt);
Dprintf((1,"closing server-socket skt#%d\n",srvr));
close(srvr);

Rdbg(("Server"));
}

/* --------------------------------------------------------------------- */
/* Write: this function insures that the requested write actually completely occurs {{{2 */
void Write(
  int     skt,    
  void   *mesg,   
  size_t  sendlen)
{
int   ret;
void *m;

Edbg(("Write(skt#%d,%smesg,sendlen=%u)",
  skt,
  mesg? "" : "null ",
  sendlen));

m= mesg;
do {
	ret = write(skt,m,sendlen);
	if(ret < 0) {
		printf("Write: %s\n",strerror(errno));
		Rdbg(("Write : failed"));
		return;
		}
	sendlen -= ret;
	m       += ret;
	Dprintf((1,"wrote %lu bytes, %lu left to write\n",
	  ret,
	  sendlen));
	} while(sendlen > 0);

Rdbg(("Write"));
}

/* --------------------------------------------------------------------- */
/* Read: this function reads requested qty of bytes via socket {{{2 */
void Read(
  int      skt,     /* socket descriptor                                             */
  size_t   recvlen, /* qty of bytes to be Read()                                     */
  void    *pbuf,    /* pointer to receiving buffer                                   */
  size_t  *buflen)  /* if buflen is null, then Read() will never (re-)allocate *pbuf */
{
ssize_t  qtyread;
char    *b;
char    *buf;

Edbg(("Read(skt#%d,recvlen=%lu,%spbuf,%sbuflen=%lu)",
  skt,
  recvlen,
  pbuf?   ""      : "null ",
  buflen? ""      : "null ",
  buflen? *buflen : 0L));

/* sanity checks */
if(buflen && !pbuf) {
	error(XTDIO_ERROR,"(Read) pbuf is null when buflen is not null\n");
	Rdbg(("Read : failure"));
	return;
	}

/* (re-)allocate *buf if necessary */
if(buflen) {
	buf= *((char **) pbuf);
	if(*buflen < recvlen) {
		if(buf) buf = (char *) realloc(buf,recvlen);
		else    buf = (char *) calloc(recvlen,sizeof(char));
		*buflen          = recvlen;
		*((char **)pbuf) = buf;
		/* sanity check */
		if(!buf) {
			error(XTDIO_ERROR,"(Read) buf is null (%sbuflen=%lu) after (re-)allocation\n",
			  buflen? ""     : "null ",
			  buflen? buflen : 0l);
			Rdbg(("Read : failure"));
			return;
			}
		}
	}
else buf= pbuf;

/* receive the bytes */
b= buf;
do {
	qtyread= read(skt,b,recvlen);
	if(qtyread < 0) {
		error(XTDIO_ERROR,"(Read) read() failed with: %s\n",strerror(errno));
		Rdbg(("Read : failure"));
		return;
		}
	else if(qtyread == 0) sleep(1);
	recvlen -= qtyread;
	b       += qtyread;
	Dprintf((1,"read %lu bytes, %lu left to read\n",
	  qtyread,
	  recvlen));
	} while(recvlen > 0);

Rdbg(("Read : %lu bytes",(size_t) (b-buf)));
}

/* ===================================================================== */
/* Modelines: {{{1
 *  vim: fdm=marker
 */
