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

// message command;
enum CMD
{
    CMD_LOGIN,
    CMD_LOGOUT,
    CMD_ERROR
};

// message header;
struct DataHeader
{
    short command;
    short length;
};

// login data package;
struct Login
{
    char userName[32];
    char passWord[32];
};

// login result data package;
struct LoginResult
{
    int result;
};

// logout data package;
struct Logout
{
    char userName[32];
};

// logout result data package;
struct LogoutResult
{
    int  result;
};

int main(int argc, char* argv[])
{
    // Initiates use of the Winsock DLL by a process;
    WORD    vers = MAKEWORD(2, 2); // WORD wVersionRequested;
    WSADATA data = { 0 };          // LPWSADATA lpWSAData;
    WSAStartup(vers, &data);

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

    char cmdBuffer[128] = { 0 };
    while (true)
    {
        memset(cmdBuffer, 0, sizeof(cmdBuffer));
        scanf("%s", cmdBuffer);

        if (0 == strcmp(cmdBuffer, "exit"))
        {
            break;
        }
        else if (0 == strcmp(cmdBuffer, "login"))
        {
            DataHeader headerLogin = { CMD_LOGIN, sizeof(Login) };
            Login dataLogin = { "Steven Paul Jobs", "redhat" };
            // 3. send login header.
            if (SOCKET_ERROR == send(serverSocket, (const char*)&headerLogin, sizeof(DataHeader), 0))
            {
                printf("failed: send login header error.\n");
            }
            else
            {
                printf("socket send login header: %d <cmd>, %d <length>.\n", headerLogin.command, headerLogin.length);
                // 3. send login data.
                if (SOCKET_ERROR == send(serverSocket, (const char*)&dataLogin, sizeof(Login), 0))
                {
                    printf("failed: send login data error.\n");
                }
                else
                {
                    printf("socket send login data: %s <username>, %s <password>.\n", dataLogin.userName, dataLogin.passWord);
                }
            }
            DataHeader headerLoginResult = { 0 };
            LoginResult dataLoginResult = { 0 };
            // 4. receive login result header.
            int headerCount = recv(serverSocket, (char*)&headerLoginResult, sizeof(DataHeader), 0);
            if (headerCount <= 0)
            {
                (SOCKET_ERROR == headerCount) ? printf("failed: recv login header error.\n") : printf("failed: recv login header close.\n");
                break;
            }
            else
            {
                printf("socket recv login result header: %d <cmd>, %d <length>.\n", headerLoginResult.command, headerLoginResult.length);
                // 4. receive login result data.
                int dataCount = recv(serverSocket, (char*)&dataLoginResult, sizeof(LoginResult), 0);
                if (dataCount <= 0)
                {
                    (SOCKET_ERROR == dataCount) ? printf("failed: recv login result data error.\n") : printf("failed: recv login result data close.\n");
                }
                else
                {
                    printf("socket recv login result data: %d <result>.\n", dataLoginResult.result);
                }
            }
        }
        else if (0 == strcmp(cmdBuffer, "logout"))
        {
            DataHeader headerLogout = { CMD_LOGOUT, sizeof(Logout) };
            Logout dataLogout = { "Steven Paul Jobs" };
            // 3. send logout header.
            if (SOCKET_ERROR == send(serverSocket, (const char*)&headerLogout, sizeof(DataHeader), 0))
            {
                printf("failed: send logout header error.\n");
            }
            else
            {
                printf("socket send logout header: %d <cmd>, %d <length>.\n", headerLogout.command, headerLogout.length);
                // 3. send logout data.
                if (SOCKET_ERROR == send(serverSocket, (const char*)&dataLogout, sizeof(Logout), 0))
                {
                    printf("failed: send logout data error.\n");
                }
                else
                {
                    printf("socket send logout data: %s <username>.\n", dataLogout.userName);
                }
            }
            DataHeader headerLogoutResult = { 0 };
            LoginResult dataLogoutResult = { 0 };
            // 4. receive logout result header.
            int headerCount = recv(serverSocket, (char*)&headerLogoutResult, sizeof(DataHeader), 0);
            if (headerCount <= 0)
            {
                (SOCKET_ERROR == headerCount) ? printf("failed: recv logout result header error.\n") : printf("failed: recv logout result header close.\n");
                break;
            }
            else
            {
                printf("socket recv logout result header: %d <cmd>, %d <length>.\n", headerLogoutResult.command, headerLogoutResult.length);
                // 4. receive logout result data.
                int dataCount = recv(serverSocket, (char*)&dataLogoutResult, sizeof(LoginResult), 0);
                if (dataCount <= 0)
                {
                    (SOCKET_ERROR == dataCount) ? printf("failed: recv logout result data error.\n") : printf("failed: recv logout result data close.\n");
                }
                else
                {
                    printf("socket recv logout result data: %d <result>.\n", dataLogoutResult.result);
                }
            }
        }
        else
        {
            printf("error: unknown command.\n");
        }
    }

    // 5. close a socket descriptor;
    closesocket(serverSocket);

    // Terminates use of the Winsock 2 DLL;
    WSACleanup();
    system("pause");
    return 0;
}