# class11:
all : connection packet ssl

connection : connection.c
	cc ${ILIST} connection.c ${LLIST} -o connection

packet : packet.c
	cc ${ILIST} packet.c ${LLIST} -o packet

ssl : ssl.c
	cc ${ILIST} ssl.c ${LLIST} -o ssl

clean :
	/bin/rm -f connection packet ssl *.o core.* tmp.*
