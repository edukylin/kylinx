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
        DataHeader header = { 0 };

        // 5. receive data header;
        int recvCount = recv(clientSocket, (char*)&header, sizeof(DataHeader), 0);
        if (recvCount <= 0)
        {
            (SOCKET_ERROR == recvCount) ? printf("failed: recv header error.\n") : printf("failed: recv header close.\n");
            break;
        }
        printf("socket recv header: %d <cmd>, %d <length>.\n", header.command, header.length);

        switch (header.command)
        {
            case CMD_LOGIN: // login command.
            {
                // 5. receive login data.
                Login dataLogin = { 0 };
                int recvCount = recv(clientSocket, (char*)&dataLogin, sizeof(Login), 0);
                if (recvCount <= 0)
                {
                    (SOCKET_ERROR == recvCount) ? printf("failed: recv login data error.\n") : printf("failed: recv login data close.\n");
                    break;
                }
                printf("socket recv login data: %s <username>, %s <password>.\n", dataLogin.userName, dataLogin.passWord);
                // 6. send login result header.
                DataHeader headerLoginResult = { CMD_LOGIN, sizeof(LoginResult) };
                if (SOCKET_ERROR == send(clientSocket, (const char*)&headerLoginResult, sizeof(DataHeader), 0))
                {
                    printf("failed: send login result header error.\n");
                    break;
                }
                else
                {
                    printf("socket send login result header: %d <cmd>, %d <length>.\n", headerLoginResult.command, headerLoginResult.length);
                    // 6. send login result data.
                    LoginResult dataLoginResult = { 1 };
                    if (SOCKET_ERROR == send(clientSocket, (const char*)&dataLoginResult, sizeof(LoginResult), 0))
                    {
                        printf("failed: send login result data error.\n");
                        break;
                    }
                    else
                    {
                        printf("socket send login result data: %d <result>.\n", dataLoginResult.result);
                    }
                }
                break;
            }
            case CMD_LOGOUT: // logout command.
            {
                // receive logout header.
                Logout dataLogout = { 0 };
                int recvCount = recv(clientSocket, (char*)&dataLogout, sizeof(Logout), 0);
                if (recvCount <= 0)
                {
                    (SOCKET_ERROR == recvCount) ? printf("failed: recv logout data error.\n") : printf("failed: recv logout data close.\n");
                    break;
                }
                printf("socket recv logout data: %s <username>.\n", dataLogout.userName);
                // send logout result header.
                DataHeader headerLogoutResult = { CMD_LOGOUT, sizeof(LogoutResult) };
                if (SOCKET_ERROR == send(clientSocket, (const char*)&headerLogoutResult, sizeof(DataHeader), 0))
                {
                    printf("failed: send logout result header error.\n");
                    break;
                }
                else
                {
                    printf("socket send logout result header: %d <cmd>, %d <length>.\n", headerLogoutResult.command, headerLogoutResult.length);
                    // send logout result data.
                    LogoutResult dataLogoutResult = { 1 };
                    if (SOCKET_ERROR == send(clientSocket, (const char*)&dataLogoutResult, sizeof(LogoutResult), 0))
                    {
                        printf("failed: send logout result data error.\n");
                        break;
                    }
                    else
                    {
                        printf("socket send logout result data: %d <result>.\n", dataLogoutResult.result);
                    }
                }
                break;
            }
            default: // unknown header.
            {
                DataHeader headerErrorResult = { 0, CMD_ERROR };
                printf("socket recv unknown header: %d cmd, %d length.\n", header.command, header.length);
                if (SOCKET_ERROR == send(clientSocket, (const char*)&headerErrorResult, sizeof(DataHeader), 0))
                {
                    printf("failed: send unknown result header error.\n");
                }
                else
                {
                    printf("socket send unknown result header: %d <cmd>, %d <length>.\n", headerErrorResult.command, headerErrorResult.length);
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