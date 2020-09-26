/*
	Peer file
	Author: Hrishikesh Bawane
*/

#include "main.h"
#include <netdb.h>

using namespace std;

#define MAX_BUFFER_SIZE	65536
#define MAX_PEERS		100
#define MAX_LEN			100
#define LOGERR			printf
#define LOGINFO			printf
#define TRUE			true
#define FALSE			false


const char serverIP[] = "127.0.0.1";
const int port = 3000;
struct sockaddr_in serverAddress;
struct hostent* peerDetails;
char peerBuffer[MAX_LEN];

int tcpSocketFD = 0;
int recvBytes = 0;
char buffer[MAX_BUFFER_SIZE];
char data[MAX_BUFFER_SIZE];
char fileName[MAX_LEN];

volatile bool exitFlag = FALSE;

//////////////////////////////////////////////////////////////////////////

void catchExitCondition()
{
	exitFlag = TRUE;
}

/////////////////////////////////////////////////////////////////////////

void flushStdout()
{
	LOGINFO("\n> ");
	fflush(stdout);
}

/////////////////////////////////////////////////////////////////////////

void* dataSendHandler(void* args)
{
	int pType = *((int*)args);
	bzero(buffer, MAX_BUFFER_SIZE);
	bzero(fileName, MAX_LEN);

	while (1)
	{
		flushStdout();
		scanf("%s", fileName);
		if (strcmp(fileName, "exit") == 0)
		{
			break;
		}
		else
		{
			buffer[0] = (char)(pType + 48);
			strcat(buffer, fileName);
			strcat(buffer, "\n");
			if (pType == 0)
			{
				// Attach file to be sent
				FILE* filePtr = fopen(fileName, "r+");
				if (filePtr == NULL)
				{
					LOGERR("Error: fopen()\n");
					exit(EXIT_FAILURE);
				}
				char tmpBuffer[MAX_BUFFER_SIZE];
				fread(tmpBuffer, MAX_BUFFER_SIZE, 1, filePtr);
				strcat(buffer, tmpBuffer);
				fclose(filePtr);
			}
			send(tcpSocketFD, buffer, strlen(buffer), 0);
		}
		bzero(buffer, MAX_BUFFER_SIZE);
		bzero(fileName, MAX_LEN);
	}
	catchExitCondition();
}

/////////////////////////////////////////////////////////////////////////

void* dataReceiveHandler(void* args)
{
	bzero(buffer, MAX_BUFFER_SIZE);
	bzero(data, MAX_BUFFER_SIZE);
	bzero(fileName, MAX_LEN);
	while (1)
	{
		recvBytes = recv(tcpSocketFD, buffer, MAX_BUFFER_SIZE, 0);
		if (recvBytes > 0)
		{
			int len = 0;
			while (buffer[len++] != '\n');
			snprintf(fileName, len, "%s", buffer);
			strcpy(data, buffer + len);
			LOGINFO("File Received: %s\n", fileName);
			LOGINFO("Data Received: %s\n", data);
			FILE* filePtr = fopen(fileName, "w+");
			if (filePtr == NULL)
			{
				LOGERR("Error: fopen()\n");
				exit(EXIT_FAILURE);
			}
			fprintf(filePtr, "%s", data);
			flushStdout();
			fclose(filePtr);
		}
		else if (recvBytes == 0)
		{
			break;
		}
		bzero(buffer, MAX_BUFFER_SIZE);
		bzero(data, MAX_BUFFER_SIZE);
		bzero(fileName, MAX_LEN);
	}
}

/////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[])
{
	int pType = atoi(argv[1]);
	bzero(buffer, MAX_BUFFER_SIZE);

	Socket sockPeer(serverIP, port, 1);

	tcpSocketFD = sockPeer.Create(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	sockPeer.Connect();

	sockPeer.Receive(tcpSocketFD, buffer, MAX_BUFFER_SIZE, 0);
	LOGINFO("%s", buffer);

	gethostname(peerBuffer, sizeof(peerBuffer));
	peerDetails = gethostbyname(peerBuffer);
	char* peerIP = inet_ntoa(*((in_addr*)peerDetails->h_addr_list[0]));
	sockPeer.Send(tcpSocketFD, peerIP, strlen(peerIP), 0);

	pthread_t dataSendThread;
	pthread_create(&dataSendThread, NULL, &dataSendHandler, (void*)&pType);

	pthread_t dataReceiveThread;
	pthread_create(&dataReceiveThread, NULL, &dataReceiveHandler, NULL);

	while (1)
	{
		if (exitFlag)
		{
			LOGINFO("Connection Terminated...\n");
			break;
		}
	}
	close(tcpSocketFD);

	return EXIT_SUCCESS;
}

