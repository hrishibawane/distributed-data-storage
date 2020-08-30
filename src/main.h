/*
	Main Header File
	Author: Hrishikesh Bawane
*/

#include <iostream>
#include <sys/socket.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>

using namespace std;

#ifndef MAIN_H
#define MAIN_H

#define LEN 20

class Socket
{

private:

	int sockPort;
	char ipAddress[LEN];

	int sockFileDesc;
	int sockBindStatus;
	int sockConnectStatus;
	int sockListenStatus;
	int sockAcceptStatus;
	int bytesSent;
	int bytesReceived;
	int peerNameStatus;
	socklen_t serverLen;

public:
	
	sockaddr_in serverAddress;

	Socket();

	Socket(const char*, const int, const int);
	
	int Create(int, int, int);

	int Bind();

	int Connect();

	int Listen(const int);

	int Accept();

	ssize_t Send(int, const void*, size_t, int);

	ssize_t Receive(int, void*, size_t, int);

	ssize_t Read(int, void*, size_t);

	ssize_t Write(int, const void*, size_t);

	int GetPeerName(int);

	~Socket();
};

#endif
