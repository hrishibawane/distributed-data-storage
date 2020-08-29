
#include <iostream>
using namespace std;

int main()
{
	char fileName[50];
	scanf("%s", fileName);
	FILE* fp = fopen(fileName, "r+");
	if (fp == NULL)
	{
		printf("Error: fopen()\n");
		exit(1);
	}

	char buffer[1000];
	fread(buffer, 1000, 1, fp);
	printf("\n%s\n", buffer);

}
