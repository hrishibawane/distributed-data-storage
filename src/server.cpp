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

using namespace std;

#define MAX_BUFFER_SIZE	10000
#define MAX_CLIENTS		50
#define MAX_LEN			15
#define LOGERR			printf
#define LOGINFO			printf


struct sockaddr_in serverAddress;
struct sockaddr_in clientAddress;
fd_set readFD;
fd_set writeFD;
const char ipAddress[] = "0.0.0.0";
const int port = 3000;

int tcpListenFD = 0;
int clientFD[MAX_CLIENTS];
int bindStatus = 0;
int listenStatus = 0;
int selectStatus = 0;
int sendStatus = 0;
int readBytes = 0;
char buffer[MAX_BUFFER_SIZE];

const char connectionError[][MAX_LEN] = {"socket()", "bind()", "listen()", "select()", "accept()", "send()", "recv()"};
const char successMsg[] = "You have successfully connected to server...\n";

////////////////////////////////////////////////////////////////////////////////

void logErrors(int code)
{
	LOGERR("ERROR: %s\n", connectionError[code]);
	exit(EXIT_FAILURE);
}

////////////////////////////////////////////////////////////////////////////////

void sendToAllClients()
{
	for (int cli = 0; cli < MAX_CLIENTS; cli++)
	{
		if (clientFD[cli] != 0)
		{
			send(clientFD[cli], buffer, strlen(buffer), 0);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////

int main(int argc, char* args[])
{
	memset(clientFD, 0, sizeof(int));
	bzero(buffer, MAX_BUFFER_SIZE);

	try
	{
		tcpListenFD = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (tcpListenFD == -1)
		{	
			throw(0);
		}

		serverAddress.sin_family = AF_INET;
		serverAddress.sin_addr.s_addr = INADDR_ANY;
		serverAddress.sin_port = htons(port);

		bindStatus = bind(tcpListenFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
		if (bindStatus == -1)
		{
			throw(1);
		}

		listenStatus = listen(tcpListenFD, MAX_CLIENTS);
		if (listenStatus == -1)
		{
			throw(2);
		}
	}
	catch (int code)
	{
		logErrors(code);
	}
	
	LOGINFO("Server ready on port %d....\n", port);

	while (1)
	{
		FD_ZERO(&readFD);					// clear read socket set
		FD_SET(tcpListenFD, &readFD);		// add tcpListen to read set
		bzero(buffer, MAX_BUFFER_SIZE);

		int maxSocketDesc = tcpListenFD;
		int socketDesc = 0;

		for (int cli = 0; cli < MAX_CLIENTS; cli++)
		{
			socketDesc = clientFD[cli];

			if (socketDesc > 0)
			{
				FD_SET(socketDesc, &readFD);
			}

			maxSocketDesc = max(socketDesc, maxSocketDesc);
		}

		selectStatus = select(maxSocketDesc + 1, &readFD, NULL, NULL, NULL);
		if (selectStatus == -1)
		{
			logErrors(3);
		}

		socklen_t serverAddrLen = sizeof(serverAddress);

		// Incoming connection
		if (FD_ISSET(tcpListenFD, &readFD))
		{
			int newConnection = accept(tcpListenFD, (struct sockaddr*)&serverAddress, &serverAddrLen);
			if (newConnection == -1)
			{
				logErrors(4);
			}

			LOGINFO("New Connection:\t socket fd: %d\t IP: %s\t port: %d\n", newConnection, inet_ntoa(serverAddress.sin_addr), ntohs(serverAddress.sin_port));

			sendStatus = send(newConnection, successMsg, strlen(successMsg), 0);
			if (sendStatus == -1)
			{
				logErrors(5);
			}

			LOGINFO("Welcome message sent successfully\n");
			
			for (int cli = 0; cli < MAX_CLIENTS; cli++)
			{
				if (clientFD[cli] == 0)
				{
					clientFD[cli] = newConnection;
					LOGINFO("New Client added to list as %d\n", cli);
					break;
				}
			}
		}

		for (int cli = 0; cli < MAX_CLIENTS; cli++)
		{
			socketDesc = clientFD[cli];

			if (FD_ISSET(socketDesc, &readFD))
			{
				// Check if incoming message or client is disconnecting
				readBytes = read(socketDesc, buffer, MAX_BUFFER_SIZE);
				if (readBytes == 0)
				{
					getpeername(socketDesc, (struct sockaddr*)&serverAddress, &serverAddrLen);
					LOGINFO("Client Disconnected:\t IP: %s\t port: %d\n", inet_ntoa(serverAddress.sin_addr), ntohs(serverAddress.sin_port));
					close(socketDesc);
					clientFD[cli] = 0;
				}
				else
				{
					buffer[readBytes] = '\0';
					LOGINFO("%s", buffer);
					sendToAllClients();
					bzero(buffer, MAX_BUFFER_SIZE);
				}
			}
		}

	}
	return EXIT_SUCCESS;
}











