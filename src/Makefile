# Makefile

all: program

program: main.o server.o peer.o
	g++ main.o server.o -o server
	g++ main.o peer.o -lpthread -o peer
	mv server peer ../bin
	rm -f *.o

# Only for server
server: main.o server.o
	rm -f ../bin/server
	g++ main.o server.o -o server
	mv main.o server.o ../obj
	mv server ../bin

# Only for peer
peer: main.o peer.o
	rm -f ../bin/peer
	g++ main.o peer.o -lpthread -o peer
	mv main.o peer.o ../obj
	mv peer ../bin

main.o:
	g++ -c main.cpp

server.o:
	g++ -c server.cpp

peer.o:
	g++ -c peer.cpp

clean:
	rm -f ../bin

