/* 
	Server File
	Author: Hrishikesh Bawane
*/

#include "main.h"
#include <map>
#include <vector>

using namespace std;

#define MAX_BUFFER_SIZE	65536
#define MAX_CLIENTS		100
#define MAX_LEN			100
#define LOGERR			printf
#define LOGINFO			printf
#define TRUE			true
#define FALSE			false

typedef struct
{
	int sockFD;
	char IP[MAX_LEN];
} Client;

fd_set readFD;
fd_set writeFD;
const char ipAddress[] = "0.0.0.0";
const int port = 3000;

int tcpListenFD = 0;
Client* clients[MAX_CLIENTS];
int readBytes = 0;
char buffer[MAX_BUFFER_SIZE];
char data[MAX_BUFFER_SIZE];
char fileName[MAX_LEN];
char hostIP[MAX_LEN];
const char successMsg[] = "You have successfully connected to server...\n";

map<string, vector<string>> locTable;
Client* currClients[MAX_CLIENTS];

////////////////////////////////////////////////////////////////////////////////

void sendToAllClients(Socket& sockServer)
{
	bzero(data, MAX_BUFFER_SIZE);
	bzero(fileName, MAX_LEN);
	int len = 0;
	int cClients = 0;

	while (buffer[len++] != '\n');
	snprintf(fileName, len, "%s", buffer);
	strcpy(data, buffer + len);

	for (int nCli = 0; nCli < MAX_CLIENTS; nCli++)
	{
		if (clients[nCli] != NULL)
		{
			currClients[cClients++] = clients[nCli];
		}
	}

	LOGINFO("Current clients: %d\n", cClients);
	int block = strlen(data) / cClients;
	int curr = 0;

	for (int nCli = 0; nCli < cClients; nCli++)
	{
		// send data
		char tBuffer[MAX_BUFFER_SIZE];
		strcpy(tBuffer, fileName);
		strcat(tBuffer, "\n");
		strncat(tBuffer, data + curr, block);
		sockServer.Send(currClients[nCli]->sockFD, tBuffer, strlen(tBuffer), 0);
		curr += block;
		string fName(fileName);
		locTable[fName].push_back(currClients[nCli]->IP);
		LOGINFO("Sent to %d: %s\n", currClients[nCli]->sockFD, tBuffer);
		bzero(tBuffer, MAX_BUFFER_SIZE);
	}
	bzero(buffer, MAX_BUFFER_SIZE);
	bzero(data, MAX_BUFFER_SIZE);
	bzero(fileName, MAX_LEN);
}

////////////////////////////////////////////////////////////////////////////////

int main(int argc, char* args[])
{
	memset(currClients, 0, sizeof(int));
	bzero(buffer, MAX_BUFFER_SIZE);

	Socket sockServer(ipAddress, port, 0);
	tcpListenFD = sockServer.Create(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	sockServer.Bind();
	sockServer.Listen(MAX_CLIENTS);

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
			socketDesc = clients[nClient] == NULL ? 0 : clients[nClient]->sockFD;
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
			int newConnection = sockServer.Accept();

			LOGINFO("New Connection:\t socket fd: %d\n", newConnection);

			sockServer.Send(newConnection, successMsg, strlen(successMsg), 0);
			LOGINFO("Welcome message sent successfully\n");

			sockServer.Receive(newConnection, hostIP, MAX_LEN, 0);
			LOGINFO("IP: %s\n", hostIP);
			
			for (int nClient = 0; nClient < MAX_CLIENTS; nClient++)
			{
				if (clients[nClient] == NULL)
				{
					clients[nClient] = new Client;
					clients[nClient]->sockFD = newConnection;
					strcpy(clients[nClient]->IP, hostIP);
					LOGINFO("New Client added to list as %d\n", nClient);
					break;
				}
			}
			bzero(hostIP, MAX_LEN);
		}

		for (int nClient = 0; nClient < MAX_CLIENTS; nClient++)
		{
			socketDesc = clients[nClient] == NULL ? 0 : clients[nClient]->sockFD;

			if (FD_ISSET(socketDesc, &readFD))
			{
				// Check if incoming message or client is disconnecting
				readBytes = sockServer.Read(socketDesc, buffer, MAX_BUFFER_SIZE);
				if (readBytes == 0)
				{
					sockServer.GetPeerName(socketDesc);
					LOGINFO("Client Disconnected:\t ID: %d\t sockfd: %d\t IP: %s\n", nClient, socketDesc, clients[nClient]->IP);
					close(socketDesc);
					clients[nClient] = NULL;
				}
				else
				{
					LOGINFO("Client %d: %s", nClient, buffer);
					sendToAllClients(sockServer);
				}
			}
		}
	}
	return EXIT_SUCCESS;
}











