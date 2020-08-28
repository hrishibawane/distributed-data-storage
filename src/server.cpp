/* 
	Server File
	Author: Hrishikesh Bawane
*/

#include <iostream>
#include <sys/socket.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include "main.h"

using namespace std;

#define MAX_BUFFER_SIZE	10000
#define MAX_CLIENTS		100
#define LOGERR			printf
#define LOGINFO			printf

fd_set readFD;
fd_set writeFD;
const char ipAddress[] = "0.0.0.0";
const int port = 3000;

int tcpListenFD = 0;
int clientFD[MAX_CLIENTS];
int readBytes = 0;
char buffer[MAX_BUFFER_SIZE];
const char successMsg[] = "You have successfully connected to server...\n";

////////////////////////////////////////////////////////////////////////////////

void sendToAllClients(Socket& sockObj)
{
	for (int nClient = 0; nClient < MAX_CLIENTS; nClient++)
	{
		if (clientFD[nClient] != 0)
		{
			sockObj.Send(clientFD[nClient], buffer, strlen(buffer), 0);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////

int main(int argc, char* args[])
{
	memset(clientFD, 0, sizeof(int));
	bzero(buffer, MAX_BUFFER_SIZE);

	Socket sockObj(ipAddress, port);
	tcpListenFD = sockObj.Create(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	sockObj.Bind();
	sockObj.Listen(MAX_CLIENTS);

	LOGINFO("Server ready on port %d....\n", port);

	while (1)
	{
		FD_ZERO(&readFD);					// clear read socket set
		FD_SET(tcpListenFD, &readFD);		// add tcpListen to read set
		bzero(buffer, MAX_BUFFER_SIZE);

		int maxSocketDesc = tcpListenFD;
		int socketDesc = 0;

		for (int nClient = 0; nClient < MAX_CLIENTS; nClient++)
		{
			socketDesc = clientFD[nClient];
			if (socketDesc > 0)
			{
				FD_SET(socketDesc, &readFD);
			}
			maxSocketDesc = max(socketDesc, maxSocketDesc);
		}

		select(maxSocketDesc + 1, &readFD, NULL, NULL, NULL);

		// Incoming connection
		if (FD_ISSET(tcpListenFD, &readFD))
		{
			int newConnection = sockObj.Accept();

			LOGINFO("New Connection:\t socket fd: %d\n", newConnection);

			sockObj.Send(newConnection, successMsg, strlen(successMsg), 0);

			LOGINFO("Welcome message sent successfully\n");
			
			for (int nClient = 0; nClient < MAX_CLIENTS; nClient++)
			{
				if (clientFD[nClient] == 0)
				{
					clientFD[nClient] = newConnection;
					LOGINFO("New Client added to list as %d\n", nClient);
					break;
				}
			}
		}

		for (int nClient = 0; nClient < MAX_CLIENTS; nClient++)
		{
			socketDesc = clientFD[nClient];

			if (FD_ISSET(socketDesc, &readFD))
			{
				// Check if incoming message or client is disconnecting
				readBytes = sockObj.Read(socketDesc, buffer, MAX_BUFFER_SIZE);
				if (readBytes == 0)
				{
					sockObj.GetPeerName(socketDesc);
					LOGINFO("Client Disconnected:\t ID: %d\t sockfd: %d\n", nClient, socketDesc);
					close(socketDesc);
					clientFD[nClient] = 0;
				}
				else
				{
					LOGINFO("Client %d: %s", nClient, buffer);
					sendToAllClients(sockObj);
					bzero(buffer, MAX_BUFFER_SIZE);
				}
			}
		}
	}
	return EXIT_SUCCESS;
}











