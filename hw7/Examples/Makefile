# Makefile for Lab#7
OBJ= hw7.o hw7c1.o hw7c2.o
EXE= hw7 hw7c1 hw7c2 

all: $(EXE)

hw7: hw7.c
	gcc -I drchipDir hw7.c drchip -o hw7 

hw7c1: hw7c1.c
	gcc -Wall hw7c1.c -o hw7c1 

hw7c2: hw7c2.c
	gcc -Wall hw7c2.c -o hw7c2 

clean:
	/bin/rm -f *.o $(EXE)
