/* select.c: this is pseudo-code illustrating use of select */
int maxfd;
maxfd should be the maximum file descriptor value

fd_set mask;
fd_set rmask;
int    server;

server = ... code to open server...
maxfd  = server;
FD_SET(server,&mask); // enter the server into the mask
while(1) {
	rmask= mask;
	ret= select(maxfd+1,&rmask,NULL,NULL,NULL); // will block until some fd in the mask has something to read
	if(ret > 0) {                               // something has shown up for reading
		if(FD_ISSET(server,rmask)) {            // the server socket has something to read...
			...accept new client...
			FD_SET(client,&rmask);              // enter client into mask
			if(client >= maxfd) maxfd= client;  // keep maxfd current
			...enter client into a linked list...
			}
		else {
		// check through list of clients
		... use FD_ISSET on clients...          // check if client(s) have something to read
		}
	else if(ret < 0) { /* failure handling */
		}
	else { /* occurs when server is non-blocking */
		}
	}
