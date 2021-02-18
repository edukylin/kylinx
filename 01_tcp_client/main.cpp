#define WIN32_LEAN_AND_MEAN             // WinSock2.h
#define _WINSOCK_DEPRECATED_NO_WARNINGS // inet_ntoa()

#include <stdio.h>
#include <Windows.h>
#include <WinSock2.h>

// Properties >> Linker >> Input >> Additional_Dependencies
#pragma comment(lib, "ws2_32.lib")

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

    // 3. receive a message from a socket or send a message on a socket;
    char receiveBuffer[128] = { 0 };
    memset(receiveBuffer, 0, sizeof(receiveBuffer));
    if (SOCKET_ERROR == recv(serverSocket, receiveBuffer, sizeof(receiveBuffer), 0))
    {
        printf("failed: recv error.\n");
    }
    else
    {
        printf("socket receive successful: %s\n", receiveBuffer);
    }

    // 4. close a socket descriptor;
    closesocket(serverSocket);

    // ********** End socket programming. ********** //

    // Terminates use of the Winsock 2 DLL;
    WSACleanup();
    getchar();

    return 0;
}