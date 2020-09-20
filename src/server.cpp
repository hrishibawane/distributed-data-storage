/* 
	Server File
	Author: Hrishikesh Bawane
*/

#include "main.h"
#include <map>
#include <vector>

using namespace std;

#define MAX_BUFFER_SIZE	65536
#define MAX_PEERS		100
#define MAX_LEN			100
#define LOGERR			printf
#define LOGINFO			printf
#define TRUE			true
#define FALSE			false

typedef struct
{
	int sockFD;
	char IP[MAX_LEN];
} Peer;

fd_set readFD;
fd_set writeFD;
const char ipAddress[] = "0.0.0.0";
const int port = 3000;

int tcpListenFD = 0;
Peer* peers[MAX_PEERS];
int readBytes = 0;
char buffer[MAX_BUFFER_SIZE];
char data[MAX_BUFFER_SIZE];
char fileName[MAX_LEN];
char hostIP[MAX_LEN];
const char successMsg[] = "You have successfully connected to server...\n";

Peer* currPeers[MAX_PEERS];
map<string, vector<string>> locTable;

////////////////////////////////////////////////////////////////////////////////

void retrieveFile(Socket& sockServer, int clientID)
{
	bzero(buffer, MAX_BUFFER_SIZE);
	bzero(data, MAX_BUFFER_SIZE);
	bzero(fileName, MAX_LEN);
	readBytes = sockServer.Read(peers[clientID]->sockFD, fileName, MAX_LEN);
	printf("Filename: %s\n", fileName);
	return;
}

///////////////////////////////////////////////////////////////////////////////

void sendToAllPeers(Socket& sockServer)
{
	bzero(data, MAX_BUFFER_SIZE);
	bzero(fileName, MAX_LEN);
	int len = 0;
	int cPeers = 0;

	while (buffer[len++] != '\n');
	snprintf(fileName, len, "%s", buffer);
	strcpy(data, buffer + len);

	for (int nCli = 0; nCli < MAX_PEERS; nCli++)
	{
		if (peers[nCli] != NULL)
		{
			currPeers[cPeers++] = peers[nCli];
		}
	}

	LOGINFO("Current peers: %d\n", cPeers);
	int block = strlen(data) / cPeers;
	int curr = 0;
	string fName(fileName);

	for (int nCli = 0; nCli < cPeers; nCli++)
	{
		char tBuffer[MAX_BUFFER_SIZE];
		strcpy(tBuffer, fileName);
		strcat(tBuffer, "\n");
		strncat(tBuffer, data + curr, block);
		sockServer.Send(currPeers[nCli]->sockFD, tBuffer, strlen(tBuffer), 0);
		curr += block;
		locTable[fName].push_back(currPeers[nCli]->IP);
		LOGINFO("Sent to %d: %s\n", currPeers[nCli]->sockFD, tBuffer);
		bzero(tBuffer, MAX_BUFFER_SIZE);
	}

	LOGINFO("File %s is stored at:\n", fileName);
	for (string ip : locTable[fName])
	{
		LOGINFO("%s\n", ip.c_str());
	}

	bzero(buffer, MAX_BUFFER_SIZE);
	bzero(data, MAX_BUFFER_SIZE);
	bzero(fileName, MAX_LEN);
}

////////////////////////////////////////////////////////////////////////////////

int main(int argc, char* args[])
{
	memset(currPeers, 0, sizeof(int));
	bzero(buffer, MAX_BUFFER_SIZE);

	Socket sockServer(ipAddress, port, 0);
	tcpListenFD = sockServer.Create(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	sockServer.Bind();
	sockServer.Listen(MAX_PEERS);

	LOGINFO("Server ready on port %d....\n", port);

	while (1)
	{
		FD_ZERO(&readFD);					// clear read socket set
		FD_SET(tcpListenFD, &readFD);		// add tcpListen to read set
		bzero(buffer, MAX_BUFFER_SIZE);

		int maxSocketDesc = tcpListenFD;
		int socketDesc = 0;

		for (int nPeer = 0; nPeer < MAX_PEERS; nPeer++)
		{
			socketDesc = peers[nPeer] == NULL ? 0 : peers[nPeer]->sockFD;
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

			// Handshake
			sockServer.Send(newConnection, successMsg, strlen(successMsg), 0);
			LOGINFO("Welcome message sent successfully\n");

			sockServer.Receive(newConnection, hostIP, MAX_LEN, 0);
			LOGINFO("IP: %s\n", hostIP);
			
			for (int nPeer = 0; nPeer < MAX_PEERS; nPeer++)
			{
				if (peers[nPeer] == NULL)
				{
					peers[nPeer] = new Peer;
					peers[nPeer]->sockFD = newConnection;
					strcpy(peers[nPeer]->IP, hostIP);
					LOGINFO("New Connection added to list at %d:\t fd: %d\t ip: %s\n", nPeer, peers[nPeer]->sockFD, peers[nPeer]->IP);
					break;
				}
			}
			bzero(hostIP, MAX_LEN);
		}

		for (int nPeer = 0; nPeer < MAX_PEERS; nPeer++)
		{
			socketDesc = peers[nPeer] == NULL ? 0 : peers[nPeer]->sockFD;

			if (FD_ISSET(socketDesc, &readFD))
			{
				// Check if incoming message or peer is disconnecting
				readBytes = sockServer.Read(socketDesc, buffer, MAX_BUFFER_SIZE);
				if (readBytes == 0)
				{
					sockServer.GetPeerName(socketDesc);
					LOGINFO("Peer Disconnected:\t ID: %d\t sockfd: %d\t IP: %s\n", nPeer, socketDesc, peers[nPeer]->IP);
					close(socketDesc);
					peers[nPeer] = NULL;
				}
				else if (strcmp(buffer, "GET\n") == 0)
				{
					LOGINFO("Client %d requesting for file\n", nPeer);
					retrieveFile(sockServer, nPeer);
				}
				else
				{
					LOGINFO("Peer %d sending file: %s\n", nPeer, buffer);
					sendToAllPeers(sockServer);
				}
			}
		}
	}
	return EXIT_SUCCESS;
}


