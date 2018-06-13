// Sockets.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include<pthread.h>
#include <conio.h>

#pragma warning(disable:4996)

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27016"

//void *start_Client(void *)
char *send_request(char *message, char* username)
{
	/*while (true)
	{*/
	WSADATA wsaData;
	SOCKET ConnectSocket = INVALID_SOCKET;
	struct addrinfo *result = NULL,
		*ptr = NULL,
		hints;
	char *sendbuf = "this is a test";
	char recvbuf[DEFAULT_BUFLEN];
	int iResult;
	int recvbuflen = DEFAULT_BUFLEN;

	// Validate the parameters
	/*if (argc != 2) {
	printf("usage: %s server-name\n", argv[0]);
	return 1;
	}*/

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		return NULL;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// Resolve the server address and port
	iResult = getaddrinfo("localhost", DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return NULL;
	}

	// Attempt to connect to an address until one succeeds
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

		// Create a SOCKET for connecting to server
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
			ptr->ai_protocol);
		if (ConnectSocket == INVALID_SOCKET) {
			printf("socket failed with error: %ld\n", WSAGetLastError());
			WSACleanup();
			return NULL;
		}

		// Connect to server.
		iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
			continue;
		}
		break;
	}

	freeaddrinfo(result);

	if (ConnectSocket == INVALID_SOCKET) {
		printf("Unable to connect to server!\n");
		WSACleanup();
		return NULL;
	}
	char msg[DEFAULT_BUFLEN];
	if (strcmp(message, "sync") == 0)
	{
		strcpy(msg, message);
	}
	else{
		strcpy(msg, username);
		strcat(msg, ":");
		strcat(msg, message);
	}
	iResult = send(ConnectSocket, msg, (int)strlen(msg), 0);
	if (iResult == SOCKET_ERROR) {
		printf("send failed with error: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return NULL;
	}

	printf("Bytes Sent: %ld\n", iResult);
	printf("Data sent: %s\n", sendbuf);
	// shutdown the connection since no more data will be sent
	iResult = shutdown(ConnectSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return NULL;
	}

	int flag = 1;

	// Receive until the peer closes the connection
	do {

		iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0){
			printf("Bytes received: %d\n", iResult);
			printf("Data recieved  %s\n", recvbuf);
			/*char *resp = (char *)malloc(sizeof(char) * iResult + 1);
			strcpy(resp, recvbuf);*/
			if (strcmp(msg, "sync") == 0)
			{
				printf("\nSyncing....");
				int i = 0;
				FILE *fptr = fopen("db.nosql", "rb+");
				if (fptr == NULL)
					fptr = fopen("db.nosql", "wb");
				if (flag == 1)
					i = sizeof(int);
				flag = 0;
				for (i; i < recvbuflen; i++)
				{
					fputc(recvbuf[i],fptr);
				}
				fclose(fptr);
			}
			recvbuf[iResult] = '\0';
			return recvbuf;
		}
		else if (iResult == 0)
			printf("Connection closed\n");
		else
			printf("recv failed with error: %d\n", WSAGetLastError());

	} while (iResult > 0);
	//}
	//free(msg);
	// cleanup
	closesocket(ConnectSocket);
	WSACleanup();
	//}
}
