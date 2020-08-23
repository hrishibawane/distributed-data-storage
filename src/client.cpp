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

#define MAX_BUFFER_SIZE		0xFFFF
#define MAX_CLIENTS			50
#define MAX_LEN				100
#define LOGERR				printf
#define LOGINFO				printf


const char serverIP[] = "127.0.0.1";
const int port = 3000;
struct sockaddr_in serverAddress;

int socketFD = 0;
int connStatus = 0;
int recvBytes = 0;
char buffer[MAX_BUFFER_SIZE];
char data[MAX_LEN];

const char connectionError[][MAX_LEN] = {"socket()", "connect()", "send()"};

//////////////////////////////////////////////////////////////////////////

void logErrors(int code)
{
	LOGERR("ERROR: %s\n", connectionError[code]);
	exit(EXIT_FAILURE);
}

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

	
	while (1)
	{
		bzero(buffer, MAX_BUFFER_SIZE);
		bzero(data, MAX_LEN);

		recvBytes = recv(socketFD, buffer, MAX_BUFFER_SIZE, 0);
		if (recvBytes > 0)
		{
			LOGINFO("%s", buffer);
		}
		
		LOGINFO("\n> ");
		fgets(buffer, MAX_BUFFER_SIZE, stdin);
		if (strcmp(buffer, "exit") == 0)
		{
			break;
		}
		else
		{
			sprintf(data, "%s\n", buffer);
			send(socketFD, data, strlen(data), 0);
		}

	}
	
	
	return EXIT_SUCCESS;
}











