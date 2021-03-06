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
    CMD_LOGIN_RESULT,
    CMD_LOGOUT,
    CMD_LOGOUT_RESULT,
    CMD_ERROR
};

// message header;
struct DataHeader
{
    short command;
    short length;
};

// login data package;
struct Login : public DataHeader
{
public:
    char userName[32];
    char passWord[32];
public:
    Login()
    {
        command = CMD_LOGIN;
        length = sizeof(Login);
    }
};

// login result data package;
struct LoginResult : public DataHeader
{
public:
    int result;
public:
    LoginResult()
    {
        command = CMD_LOGIN_RESULT;
        length = sizeof(LoginResult);
        result = 0; // correct;
    }
};

// logout data package;
struct Logout : public DataHeader
{
public:
    char userName[32];
public:
    Logout()
    {
        command = CMD_LOGOUT;
        length = sizeof(Logout);
    }
};

// logout result data package;
struct LogoutResult : public DataHeader
{
public:
    int  result;
public:
    LogoutResult()
    {
        command = CMD_LOGOUT_RESULT;
        length = sizeof(LogoutResult);
        result = 0; // correct;
    }
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
            // 3. send login.
            Login login;
            strcpy(login.userName, "Steven Paul Jobs");
            strcpy(login.passWord, "redhat");
            if (SOCKET_ERROR == send(serverSocket, (const char*)&login, sizeof(Login), 0))
            {
                printf("failed: send login error.\n");
                break;
            }
            else
            {
                printf("socket send login: CMD_LOGIN <cmd>, %d <bytes>, %s <username>, %s <password>.\n", login.length, login.userName, login.passWord);
            }
            // 4. receive login result.
            LoginResult loginResult;
            int recvCount = recv(serverSocket, (char*)&loginResult, sizeof(LoginResult), 0);
            if (recvCount <= 0)
            {
                (SOCKET_ERROR == recvCount) ? printf("failed: recv login result error.\n") : printf("failed: recv login result close.\n");
            }
            else
            {
                printf("socket recv login result: CMD_LOGIN_RESULT <cmd>, %d <bytes>, %d <result>.\n", loginResult.length, loginResult.result);
            }
        }
        else if (0 == strcmp(cmdBuffer, "logout"))
        {
            // 3. send logout.
            Logout logout;
            strcpy(logout.userName, "Steven Paul Jobs");
            if (SOCKET_ERROR == send(serverSocket, (const char*)&logout, sizeof(Logout), 0))
            {
                printf("failed: send logout error.\n");
                break;
            }
            else
            {
                printf("socket send logout: CMD_LOGOUT <cmd>, %d <bytes>, %s <username>.\n", logout.length, logout.userName);
            }
            // 4. receive logout result.
            LogoutResult logoutResult;
            int recvCount = recv(serverSocket, (char*)&logoutResult, sizeof(LoginResult), 0);
            if (recvCount <= 0)
            {
                (SOCKET_ERROR == recvCount) ? printf("failed: recv logout result error.\n") : printf("failed: recv logout result close.\n");
            }
            else
            {
                printf("socket recv logout result: CMD_LOGOUT_RESULT <cmd>, %d <bytes>, %d <result>.\n", logoutResult.length, logoutResult.result);
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