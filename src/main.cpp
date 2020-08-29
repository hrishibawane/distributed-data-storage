/*
	Main File
	Author: Hrishikesh Bawane
*/

#include "main.h"

#define LOGERR printf
#define LOGINFO printf

using namespace std;

Socket::Socket() {}

Socket::Socket(const char* ip, const int port, const int type)
{
	strcpy(ipAddress, ip);
	sockPort = port;

	sockFileDesc = 0;
	sockBindStatus = 0;
	sockConnectStatus = 0;
	sockListenStatus = 0;
	sockAcceptStatus = 0;
	bytesSent = 0;
	bytesReceived = 0;
	peerNameStatus = 0;

	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = (type == 0) ? INADDR_ANY : inet_addr(ipAddress);
	serverAddress.sin_port = htons(sockPort);
	serverLen = sizeof(serverAddress);
}

int Socket::Create(int domain, int type, int protocol)
{
	sockFileDesc = socket(domain, type, protocol);
	if (sockFileDesc == -1)
	{
		LOGERR("Error: socket()\n");
		exit(EXIT_FAILURE);
	}
	return sockFileDesc;
}

int Socket::Bind()
{
	sockBindStatus = bind(sockFileDesc, (sockaddr*)&serverAddress, serverLen);
	if (sockBindStatus == -1)
	{
		LOGERR("Error: bind()\n");
		exit(EXIT_FAILURE);
	}
	return sockBindStatus;
}

int Socket::Connect()
{
	sockConnectStatus = connect(sockFileDesc, (sockaddr*)&serverAddress, serverLen);
	if (sockConnectStatus == -1)
	{
		LOGERR("Error: connect()\n");
		exit(EXIT_FAILURE);
	}
	return sockConnectStatus;
}

int Socket::Listen(const int maxClients)
{
	sockListenStatus = listen(sockFileDesc, maxClients);
	if (sockListenStatus == -1)
	{
		LOGERR("Error: listen()\n");
		exit(EXIT_FAILURE);
	}
	return sockListenStatus;
}

int Socket::Accept()
{
	sockAcceptStatus = accept(sockFileDesc, (sockaddr*)&serverAddress, &serverLen);
	if (sockAcceptStatus == -1)
	{
		LOGERR("Error: accept()");
		exit(EXIT_FAILURE);
	}
	return sockAcceptStatus;
}

ssize_t Socket::Send(int connFileDesc, const void* buffer, size_t bufferLen, int flags)
{
	bytesSent = send(connFileDesc, buffer, bufferLen, flags);
	if (bytesSent == -1)
	{
		LOGERR("Error: send()");
	}
	return bytesSent;
}

ssize_t Socket::Receive(int connFileDesc, void* buffer, size_t bufferLen, int flags)
{
	bytesReceived = recv(connFileDesc, buffer, bufferLen, flags);
	if (bytesReceived == -1)
	{
		LOGERR("Error: recv()");
	}
	return bytesReceived;
}

ssize_t Socket::Read(int socketFD, void* buffer, size_t count)
{
	return read(socketFD, buffer, count);
}

ssize_t Socket::Write(int socketFD, const void* buffer, size_t count)
{
	return write(socketFD, buffer, count);
}

int Socket::GetPeerName(int socketFD)
{
	peerNameStatus = getpeername(socketFD, (sockaddr*)&serverAddress, &serverLen);
	if (peerNameStatus == -1)
	{
		LOGERR("Error: getpeername()");
	}
	return peerNameStatus;
}

Socket::~Socket()
{
	close(sockFileDesc);
}

