#define WIN32_LEAN_AND_MEAN             // WinSock2.h
#define _WINSOCK_DEPRECATED_NO_WARNINGS // inet_ntoa()
#define _CRT_SECURE_NO_WARNINGS         // strcmp()

#include <Windows.h>
#include <WinSock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Properties >> Linker >> Input >> Additional_Dependencies
#pragma comment(lib, "ws2_32.lib")

struct DataPackage
{
    char name[32];
    int  age;
};

int main(int argc, char* argv[])
{
    // Initiates use of the Winsock DLL by a process;
    WORD    vers = MAKEWORD(2, 2); // WORD wVersionRequested;
    WSADATA data = { 0 };          // LPWSADATA lpWSAData;
    WSAStartup(vers, &data);

    // ********** Begin socket programming. ********** //

    // 1. create a socket for communication;
    SOCKET serverSocket = INVALID_SOCKET;
    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (INVALID_SOCKET == serverSocket)
    {
        printf("failed: socket error.\n");
    }
    else
    {
        printf("socket create successful.\n");
    }

    // 2. Initiate a connection on a socket;
    struct sockaddr_in serverAddr = { 0 };
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
    serverAddr.sin_port = htons(9981);
    if (SOCKET_ERROR == connect(serverSocket, (const sockaddr*)&serverAddr, sizeof(serverAddr)))
    {
        printf("failed: connect error.\n");
    }
    else {
        printf("socket connect successful.\n");
    }

    char recvBuffer[128] = { 0 };
    char sendBuffer[128] = { 0 };

    while (true)
    {
        memset(recvBuffer, 0, sizeof(recvBuffer));
        memset(sendBuffer, 0, sizeof(sendBuffer));

        scanf("%s", sendBuffer);
        if (0 == strcmp(sendBuffer, "exit"))
        {
            break;
        }
        else
        {
            // 3. send a message on a socket;
            if (SOCKET_ERROR == send(serverSocket, sendBuffer, strlen(sendBuffer) + 1, 0))
            {
                printf("failed: send error.\n");
            }
            else
            {
                printf("socket send successful: %s\n", sendBuffer);
            }
        }

        // 4. receive a message from a socket;
        int recvCount = recv(serverSocket, recvBuffer, sizeof(recvBuffer), 0);
        if (recvCount <= 0)
        {
            (SOCKET_ERROR == recvCount) ? printf("failed: recv error.\n") : printf("socket client close.\n");
            break;
        }
        struct DataPackage* pDataPackage = (struct DataPackage*)recvBuffer;
        printf("socket recv message: name<%s>, age<%d>.\n", pDataPackage->name, pDataPackage->age);
    }

    // 4. close a socket descriptor;
    closesocket(serverSocket);

    // ********** End socket programming. ********** //

    // Terminates use of the Winsock 2 DLL;
    WSACleanup();
    system("pause");

    return 0;
}