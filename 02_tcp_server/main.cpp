#define WIN32_LEAN_AND_MEAN             // WinSock2.h
#define _WINSOCK_DEPRECATED_NO_WARNINGS // inet_ntoa()
#define _CRT_SECURE_NO_WARNINGS         // strcpy() & strcmp()

#include <Windows.h>
#include <WinSock2.h>
#include <stdio.h>
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
    WORD    vers = MAKEWORD(2, 2);
    WSADATA data = { 0 };
    WSAStartup(vers, &data);

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

    while (true)
    {
        // 5. receive header;
        DataHeader header = { 0 };
        int recvCount = recv(clientSocket, (char*)&header, sizeof(DataHeader), 0);
        if (recvCount <= 0)
        {
            (SOCKET_ERROR == recvCount) ? printf("failed: recv header error.\n") : printf("failed: recv header close.\n");
            break;
        }

        switch (header.command)
        {
            case CMD_LOGIN: // login command.
            {
                // 5. receive login.
                Login login;
                int recvCount = recv(clientSocket, (char*)&login+sizeof(DataHeader), sizeof(Login)-sizeof(DataHeader), 0);
                if (recvCount <= 0)
                {
                    (SOCKET_ERROR == recvCount) ? printf("failed: recv login error.\n") : printf("failed: recv login close.\n");
                    break;
                }
                printf("socket recv login data: CMD_LOGIN <cmd>, %d <bytes>, %s <username>, %s <password>.\n", login.length, login.userName, login.passWord);
                // 6. send login result.
                LoginResult loginResult;
                if (SOCKET_ERROR == send(clientSocket, (const char*)&loginResult, sizeof(loginResult), 0))
                {
                    printf("failed: send login result error.\n");
                }
                else
                {
                    printf("socket send login result: CMD_LOGIN_RESULT <cmd>, %d <bytes>, %d <result>.\n", loginResult.length, loginResult.result);
                }
                break;
            }
            case CMD_LOGOUT: // logout command.
            {
                // 5. receive logout.
                Logout logout;
                int recvCount = recv(clientSocket, (char*)&logout+ sizeof(DataHeader), sizeof(Logout)-sizeof(DataHeader), 0);
                if (recvCount <= 0)
                {
                    (SOCKET_ERROR == recvCount) ? printf("failed: recv logout error.\n") : printf("failed: recv logout close.\n");
                    break;
                }
                printf("socket recv login data: CMD_LOGOUT <cmd>, %d <bytes>, %s <username>.\n", logout.length, logout.userName);
                // 6. send logout result.
                LogoutResult logoutResult;
                if (SOCKET_ERROR == send(clientSocket, (const char*)&logoutResult, sizeof(LogoutResult), 0))
                {
                    printf("failed: send logout result header error.\n");
                }
                else
                {
                    printf("socket send logout result: CMD_LOGOUT_RESULT <cmd>, %d <bytes>, %d <result>.\n", logoutResult.length, logoutResult.result);
                }
                break;
            }
            default: // unknown header.
            {
                printf("socket recv unknown header: %d <cmd>, %d <bytes>.\n", header.command, header.length);
                DataHeader errorResult = { CMD_ERROR, 0 };
                if (SOCKET_ERROR == send(clientSocket, (const char*)&errorResult, sizeof(DataHeader), 0))
                {
                    printf("failed: send unknown result error.\n");
                }
                else
                {
                    printf("socket send unknown result : CMD_ERROR <cmd>, %d <bytes>.\n", errorResult.length);
                }
                break;
            }     
        }
    }

    // 7. close client socket and listen socket;
    closesocket(clientSocket);
    closesocket(listenSocket);

    // Terminates use of the Winsock 2 DLL;
    WSACleanup();
    return 0;
}