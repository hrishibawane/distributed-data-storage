# Makefile

all: program

program: main.o server.o client.o
	g++ main.o server.o -o server
	g++ main.o client.o -lpthread -o client

main.o:
	g++ -c main.cpp

server.o:
	g++ -c server.cpp

client.o:
	g++ -c client.cpp

clean:
	rm -rf *.o
	rm -rf server client

