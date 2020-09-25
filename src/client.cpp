/*
	Client File
	Author: Hrishikesh Bawane
*/

#include "main.h"
#include <netdb.h>

using namespace std;

#define MAX_BUFFER_SIZE 65536
#define MAX_PEERS		100
#define MAX_LEN			100
#define LOGERR			printf
#define LOGINFO			printf
#define TRUE			true
#define FALSE			false

const char serverIP[] = "127.0.0.1";
const int port = 3000;
struct sockaddr_in serverAddress;
struct hostent* clientDetails;
char clientBuffer[MAX_LEN];

int tcpSocketFD = 0;
int recvBytes = 0;
char buffer[MAX_BUFFER_SIZE];
char data[MAX_BUFFER_SIZE];
char fileName[MAX_LEN];

volatile bool exitFlag = FALSE;

/////////////////////////////////////////////////////////////////////

void catchExitCondition()
{
	exitFlag = TRUE;
}

/////////////////////////////////////////////////////////////////////

void flushStdout()
{
	LOGINFO("\n> ");
	fflush(stdout);
}

/////////////////////////////////////////////////////////////////////

void* dataSendHandler(void* args)
{
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
			printf("Filename sent: %s\n", fileName);
			send(tcpSocketFD, fileName, strlen(fileName), 0);
		}
		bzero(fileName, MAX_LEN);
	}
	catchExitCondition();
}

/////////////////////////////////////////////////////////////////////

void* dataReceiveHandler(void* args)
{
}

/////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[])
{
	bzero(buffer, MAX_BUFFER_SIZE);

	Socket sockClient(serverIP, port, 1);
	
	tcpSocketFD = sockClient.Create(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	sockClient.Connect();

	sockClient.Receive(tcpSocketFD, buffer, MAX_BUFFER_SIZE, 0);
	LOGINFO("%s", buffer);

	gethostname(clientBuffer, sizeof(clientBuffer));
	clientDetails = gethostbyname(clientBuffer);
	char* clientIP = inet_ntoa(*((in_addr*)clientDetails->h_addr_list[0]));
	sockClient.Send(tcpSocketFD, clientIP, strlen(clientIP), 0);

	pthread_t dataSendThread;
	pthread_create(&dataSendThread, NULL, &dataSendHandler, NULL);

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



