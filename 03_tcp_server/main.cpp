#define WIN32_LEAN_AND_MEAN             // WinSock2.h
#define _WINSOCK_DEPRECATED_NO_WARNINGS // inet_ntoa()
#define _CRT_SECURE_NO_WARNINGS         // strcpy() & strcmp()

#include <Windows.h>
#include <WinSock2.h>
#include <stdio.h>
#include <string.h>
#include <vector>

// Properties >> Linker >> Input >> Additional_Dependencies
#pragma comment(lib, "ws2_32.lib")

// message command;
enum CMD
{
    CMD_LOGIN,
    CMD_LOGIN_RESULT,
    CMD_LOGOUT,
    CMD_LOGOUT_RESULT,
    CMD_NEW_USER_JOIN,
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

// new user join data package;
struct NewUserJoin : public DataHeader
{
public:
    int  socket;
public:
    NewUserJoin()
    {
        command = CMD_NEW_USER_JOIN;
        length = sizeof(NewUserJoin);
        socket = 0; // correct;
    }
};

// client socket vector;
std::vector<SOCKET> g_clients;

int processor(SOCKET clientSocket)
{
    // 5. receive header;
    char recvBuffer[1024] = { 0 };
    int recvCount = recv(clientSocket, (char*)&recvBuffer, sizeof(DataHeader), 0);
    if (recvCount <= 0)
    {
        (SOCKET_ERROR == recvCount) ? printf("failed: socket <%d> recv header error.\n", clientSocket) : printf("failed: socket <%d> recv header close.\n", clientSocket);
        return -1;
    }

    switch (((DataHeader*)recvBuffer)->command)
    {
        case CMD_LOGIN:
        {
            // 5. receive login;
            Login* pLogin = (Login*)recvBuffer;
            int recvCount = recv(clientSocket, recvBuffer + sizeof(DataHeader), pLogin->length - sizeof(DataHeader), 0);
            if (recvCount <= 0)
            {
                (SOCKET_ERROR == recvCount) ? printf("failed: socket <%d> recv login error.\n", clientSocket) : printf("failed: socket <%d> recv login close.\n", clientSocket);
                return -1;
            }
            printf("socket <%d> recv login data: CMD_LOGIN <cmd>, %d <bytes>, %s <username>, %s <password>.\n", clientSocket, pLogin->length, pLogin->userName, pLogin->passWord);
            // 6. send login result;
            LoginResult loginResult;
            if (SOCKET_ERROR == send(clientSocket, (const char*)&loginResult, sizeof(loginResult), 0))
            {
                printf("failed: socket <%d> send login result error.\n", clientSocket);
            }
            else
            {
                printf("socket <%d> send login result: CMD_LOGIN_RESULT <cmd>, %d <bytes>, %d <result>.\n", clientSocket, loginResult.length, loginResult.result);
            }
            break;
        }
        case CMD_LOGOUT:
        {
            // 5. receive logout;
            Logout* pLogout = (Logout*)recvBuffer;
            int recvCount = recv(clientSocket, recvBuffer + sizeof(DataHeader), pLogout->length - sizeof(DataHeader), 0);
            if (recvCount <= 0)
            {
                (SOCKET_ERROR == recvCount) ? printf("failed: socket <%d> recv logout error.\n", clientSocket) : printf("failed: socket <%d> recv logout close.\n", clientSocket);
                return -1;
            }
            printf("socket <%d> recv login data: CMD_LOGOUT <cmd>, %d <bytes>, %s <username>.\n", clientSocket, pLogout->length, pLogout->userName);
            // 6. send logout result;
            LogoutResult logoutResult;
            if (SOCKET_ERROR == send(clientSocket, (const char*)&logoutResult, sizeof(LogoutResult), 0))
            {
                printf("failed: socket <%d> send logout result header error.\n", clientSocket);
            }
            else
            {
                printf("socket <%d> send logout result: CMD_LOGOUT_RESULT <cmd>, %d <bytes>, %d <result>.\n", clientSocket, logoutResult.length, logoutResult.result);
            }
            break;
        }
        default: // unknown header.
        {
            DataHeader* pHeader = (DataHeader*)recvBuffer;
            printf("socket <%d> recv unknown header: %d <cmd>, %d <bytes>.\n", clientSocket, pHeader->command, pHeader->length);
            DataHeader errorResult = { CMD_ERROR, 0 };
            if (SOCKET_ERROR == send(clientSocket, (const char*)&errorResult, sizeof(DataHeader), 0))
            {
                printf("failed: socket <%d> send unknown result error.\n", clientSocket);
            }
            else
            {
                printf("socket <%d> send unknown result : CMD_ERROR <cmd>, %d <bytes>.\n", clientSocket, errorResult.length);
            }
            break;
        }
    }
    return 0;
}


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

    // 4. berkeley socket select programming;
    while (true)
    {
        fd_set  fdsRead, fdsWrite, fdsExcept;
        timeval timeout = { 0 };
        // clear read fdset, write fdset, and except fdset;
        FD_ZERO(&fdsRead);
        FD_ZERO(&fdsWrite);
        FD_ZERO(&fdsExcept);
        // init read fd set;
        FD_SET(listenSocket, &fdsRead);
        for (size_t i = 0; i < g_clients.size(); i++) // size_t -> unsigned int
        {
            FD_SET(g_clients[i], &fdsRead);
        }
        // select system call.
        int fdCount = select(listenSocket+1, &fdsRead, &fdsWrite, &fdsExcept, NULL);
        if (SOCKET_ERROR == fdCount)
        {
            printf("failed: select error.\n");
            break;
        }
        // accept a connection on a listen socket;
        if (FD_ISSET(listenSocket, &fdsRead))
        {
            FD_CLR(listenSocket, &fdsRead);
            struct sockaddr_in clientAddress = { 0 };
            int clientAddressLength = sizeof(clientAddress);
            SOCKET clientSocket = accept(listenSocket, (struct sockaddr*)&clientAddress, &clientAddressLength);
            printf("socket <%d> accept %s successful.\n", listenSocket, inet_ntoa(clientAddress.sin_addr));
            for (auto iter = g_clients.begin(); iter != g_clients.end(); iter++)
            {
                NewUserJoin newUserJoin;
                newUserJoin.socket = clientSocket;
                if (SOCKET_ERROR == send(*iter, (const char*)&newUserJoin, newUserJoin.length, 0))
                {
                    printf("failed: socket <%d> send new user join message error.\n", clientSocket);
                }
                else
                {
                    printf("socket <%d> send new user join message: CMD_NEW_USER_JOIN <cmd>, %d <bytes>, %d <id>.\n", clientSocket, newUserJoin.length, newUserJoin.socket);
                }
            }
            g_clients.push_back(clientSocket);
        }
        // receive and send message on a client socket;
        for (auto iter = g_clients.begin(); iter != g_clients.end();)
        {
            if (FD_ISSET(*iter, &fdsRead))
            {
                if (-1 == processor(*iter))
                {
                    iter = g_clients.erase(iter);
                    continue;
                }
            }
            iter++;
        }
    }

    // 5. close client socket and listen socket;
    closesocket(listenSocket);
    for (auto iter = g_clients.begin(); iter != g_clients.end(); iter++)
    {
        closesocket(*iter);
    }
    
    // Terminates use of the Winsock 2 DLL;
    WSACleanup();
    return 0;
}

