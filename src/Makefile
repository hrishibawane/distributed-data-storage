# Makefile

all:
	g++ server.cpp -o server
	g++ client.cpp -o client -lpthread

server:
	g++ server.cpp -o server

client:
	g++ client.cpp -o client -lpthread

clean-all:
	rm -rf server client

clean-server:
	rm -rf server

clean-client:
	rm -rf client
