# Makefile

all: program

program: main.o server.o peer.o
	g++ main.o server.o -o server
	g++ main.o peer.o -lpthread -o peer
	cp -t ../bin/ server peer
	rm *.o

main.o:
	g++ -c main.cpp
	cp main.o ../obj/

server.o:
	g++ -c server.cpp
	cp server.o ../obj/

peer.o:
	g++ -c peer.cpp
	cp peer.o ../obj/

clean:
	rm server peer

