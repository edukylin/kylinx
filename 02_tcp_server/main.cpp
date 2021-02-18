#define WIN32_LEAN_AND_MEAN             // WinSock2.h
#define _WINSOCK_DEPRECATED_NO_WARNINGS // inet_ntoa()
#define _CRT_SECURE_NO_WARNINGS         // strcpy() & strcmp()

#include <Windows.h>
#include <WinSock2.h>
#include <stdio.h>
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
    WORD    vers = MAKEWORD(2, 2);
    WSADATA data = { 0 };
    WSAStartup(vers, &data);

    // ********** Begin socket programming. ********** //

    // 1. create a socket for communication;
    SOCKET listenSocket = INVALID_SOCKET;
    listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (INVALID_SOCKET == listenSocket)
    {
        printf("failed: socket error.\n");
    }
    else
    {
        printf("socket create successful.\n");
    }

    // 2. bind a name to a socket;
    struct sockaddr_in serverAddr = { 0 };
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.S_un.S_addr = INADDR_ANY; // inet_addr("127.0.0.1");
    serverAddr.sin_port = htons(9981);
    if (SOCKET_ERROR == bind(listenSocket, (const sockaddr*)&serverAddr, sizeof(serverAddr)))
    {
        printf("failed: bind error.\n");
    }
    else {
        printf("socket bind successful.\n");
    }

    // 3. listen for connections on a socket;
    if (SOCKET_ERROR == listen(listenSocket, 20))
    {
        printf("failed: listen error.\n");
    }
    else
    {
        printf("socket listen successful.\n");
    }

    // 4. accept a connection on a socket;
    SOCKET clientSocket = INVALID_SOCKET;
    struct sockaddr_in clientAddress = { 0 };
    int clientAddressLength = sizeof(clientAddress);
    clientSocket = accept(listenSocket, (struct sockaddr*)&clientAddress, &clientAddressLength);
    if (INVALID_SOCKET == clientSocket)
    {
        printf("failed: accept error.\n");
    }
    else
    {
        printf("socket accept %s successful.\n", inet_ntoa(clientAddress.sin_addr));
    }

    char recvBuffer[128] = { 0 };
    char sendBuffer[128] = { 0 };
    while (true)
    {
        memset(recvBuffer, 0, sizeof(recvBuffer));
        memset(sendBuffer, 0, sizeof(sendBuffer));

        // 5. receive a message from a socket;
        int recvCount = recv(clientSocket, recvBuffer, sizeof(recvBuffer), 0);
        if (recvCount <= 0)
        {
            (SOCKET_ERROR == recvCount) ? printf("failed: recv error.\n") : printf("socket client close.\n");
            break;
        }

        if (0 == strcmp(recvBuffer, "getInfo"))
        {
            // 6. send a message on a socket;
            struct DataPackage dataPackage = { "Steven Paul Jobs.", 56 };
            if (SOCKET_ERROR == send(clientSocket, (const char*)&dataPackage, sizeof(dataPackage), 0))
            {
                printf("failed: send error.\n");
            }
            else
            {
                printf("socket recv message: %s\n", recvBuffer);
                printf("socket send message: name<%s>, age<%d>.\n", dataPackage.name, dataPackage.age);
            }
        }
        else
        {
            // 6. send a message on a socket;
            strcpy(sendBuffer, "I don't understand.");
            if (SOCKET_ERROR == send(clientSocket, sendBuffer, strlen(sendBuffer) + 1, 0))
            {
                printf("failed: send error.\n");
            }
            else
            {
                printf("socket recv message: %s\n", recvBuffer);
                printf("socket send message: %s\n", sendBuffer);
            }
        }
    }

    // 7. close client socket and listen socket;
    closesocket(clientSocket);
    closesocket(listenSocket);

    // ********** End socket programming. ********** //

    // Terminates use of the Winsock 2 DLL;
    WSACleanup();

    return 0;
}