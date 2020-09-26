# Makefile

CC = g++

all: program

program: main.o server.o peer.o
	$(CC) main.o server.o -o server
	$(CC) main.o peer.o -lpthread -o peer
	mv server peer ../bin
	rm -f *.o

# Only for server
server: main.o server.o
	rm -f ../bin/server
	$(CC) main.o server.o -o server
	mv main.o server.o ../obj
	mv server ../bin

# Only for peer
peer: main.o peer.o
	rm -f ../bin/peer
	$(CC) main.o peer.o -lpthread -o peer
	mv main.o peer.o ../obj
	mv peer ../bin

main.o:
	$(CC) -c main.cpp

server.o:
	$(CC) -c server.cpp

peer.o:
	$(CC) -c peer.cpp

clean:
	rm -f ../bin

