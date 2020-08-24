/*
	Client file
	Author: Hrishikesh Bawane
*/

#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>

using namespace std;

#define MAX_BUFFER_SIZE	0xFFFF
#define MAX_CLIENTS		50
#define MAX_LEN			100
#define LOGERR			printf
#define LOGINFO			printf
#define TRUE			true
#define FALSE			false


const char serverIP[] = "127.0.0.1";
const int port = 3000;
struct sockaddr_in serverAddress;

int socketFD = 0;
int connStatus = 0;
int recvBytes = 0;
int selectStatus = 0;
char buffer[MAX_BUFFER_SIZE];
char data[MAX_LEN];

const char connectionError[][MAX_LEN] = {"socket()", "connect()", "send()", "select()"};
volatile bool exitFlag = FALSE;

//////////////////////////////////////////////////////////////////////////

void logErrors(int code)
{
	LOGERR("ERROR: %s\n", connectionError[code]);
	exit(EXIT_FAILURE);
}

////////////////////////////////////////////////////////////////////////

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
	bzero(buffer, MAX_BUFFER_SIZE);
	bzero(data, MAX_LEN);

	while (1)
	{
		flushStdout();
		fgets(buffer, MAX_LEN, stdin);
		if (strcmp(buffer, "exit\n") == 0)
		{
			break;
		}
		else 
		{
			send(socketFD, buffer, strlen(buffer), 0);
		}
		bzero(buffer, MAX_BUFFER_SIZE);
		bzero(data, MAX_LEN);
	}
	catchExitCondition();
}

/////////////////////////////////////////////////////////////////////////

void* dataReceiveHandler(void* args)
{
	bzero(buffer, MAX_BUFFER_SIZE);
	
	while (1)
	{
		recvBytes = recv(socketFD, buffer, MAX_BUFFER_SIZE, 0);
		if (recvBytes > 0)
		{
			LOGINFO("%s", buffer);
			flushStdout();
		}
		else if (recvBytes == 0)
		{
			break;
		}
	}
}

/////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv)
{
	bzero(buffer, MAX_BUFFER_SIZE);

	try
	{
		socketFD = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (socketFD == -1)
		{
			throw 0;
		}
		
		serverAddress.sin_family = AF_INET;
		serverAddress.sin_addr.s_addr = inet_addr(serverIP);
		serverAddress.sin_port = htons(port);

		connStatus = connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
		if (connStatus == -1)
		{
			throw 1;
		}
	}
	catch (int code)
	{
		logErrors(code);
	}

	recv(socketFD, buffer, MAX_BUFFER_SIZE, 0);
	LOGINFO("%s", buffer);

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
	close(socketFD);
	
	return EXIT_SUCCESS;
}











