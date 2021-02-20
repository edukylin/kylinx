#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <thread>

#ifdef _WIN32 // WIN32 header file, macro, and dll;
    #define WIN32_LEAN_AND_MEAN             // WinSock2.h
    #define _WINSOCK_DEPRECATED_NO_WARNINGS // inet_ntoa()
    #define _CRT_SECURE_NO_WARNINGS         // strcmp()
    #include <Windows.h>
    #include <WinSock2.h>
    #pragma comment(lib, "ws2_32.lib")
#else
    #include <unistd.h>
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <arpa/inet.h>
    typedef int SOCKET;
    #define SOCKET_ERROR    (-1)
    #define INVALID_SOCKET  (SOCKET)(~0)
#endif

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

int processor(SOCKET serverSocket)
{
    // 5. receive header;
    char recvBuffer[1024] = { 0 };
    int recvCount = recv(serverSocket, (char*)&recvBuffer, sizeof(DataHeader), 0);
    if (recvCount <= 0)
    {
        (SOCKET_ERROR == recvCount) ? printf("failed: socket <%d> recv header error.\n", serverSocket) : printf("failed: socket <%d> recv header close.\n", serverSocket);
        return -1;
    }

    switch (((DataHeader*)recvBuffer)->command)
    {
        case CMD_LOGIN_RESULT:
        {
            // 5. receive login result;
            LoginResult* pLoginResult = (LoginResult*)recvBuffer;
            int recvCount = recv(serverSocket, recvBuffer + sizeof(DataHeader), pLoginResult->length - sizeof(DataHeader), 0);
            if (recvCount <= 0)
            {
                (SOCKET_ERROR == recvCount) ? printf("failed: socket <%d> recv login result error.\n", serverSocket) : printf("failed: socket <%d> recv login result close.\n", serverSocket);
                return -1;
            }
            printf("socket <%d> recv login result data: CMD_LOGIN_RESULT <cmd>, %d <bytes>, %d <result>.\n", serverSocket, pLoginResult->length, pLoginResult->result);
            break;
        }
        case CMD_LOGOUT_RESULT:
        {
            // 5. receive logout result;
            LogoutResult* pLogoutResult = (LogoutResult*)recvBuffer;
            int recvCount = recv(serverSocket, recvBuffer + sizeof(DataHeader), pLogoutResult->length - sizeof(DataHeader), 0);
            if (recvCount <= 0)
            {
                (SOCKET_ERROR == recvCount) ? printf("failed: socket <%d> recv logout result error.\n", serverSocket) : printf("failed: socket <%d> recv logout result close.\n", serverSocket);
                return -1;
            }
            printf("socket <%d> recv logout result data: CMD_LOGOUT_RESULT <cmd>, %d <bytes>, %d <result>.\n", serverSocket, pLogoutResult->length, pLogoutResult->result);
            break;
        }
        case CMD_NEW_USER_JOIN:
        {
            // 5. receive new user join;
            NewUserJoin* pNewUserJoin = (NewUserJoin*)recvBuffer;
            int recvCount = recv(serverSocket, recvBuffer + sizeof(DataHeader), pNewUserJoin->length - sizeof(DataHeader), 0);
            if (recvCount <= 0)
            {
                (SOCKET_ERROR == recvCount) ? printf("failed: socket <%d> recv new user join error.\n", serverSocket) : printf("failed: socket <%d> recv new user join close.\n", serverSocket);
                return -1;
            }
            printf("socket <%d> recv new user join data: CMD_NEW_USER_JOIN <cmd>, %d <bytes>, %d <id>.\n", serverSocket, pNewUserJoin->length, pNewUserJoin->socket);
            break;
        }
        default: 
        {
            // 5. receive unknown message;
            DataHeader* pHeader = (DataHeader*)recvBuffer;
            printf("socket <%d> recv unknown header: %d <cmd>, %d <bytes>.\n", serverSocket, pHeader->command, pHeader->length);
            break;
        }
    }
    return 0;
}

bool g_bRun = true;

void cmdTrhead(SOCKET socket)
{
    char cmdBuffer[128] = { 0 };
    while (true)
    {
        memset(cmdBuffer, 0, sizeof(cmdBuffer));
        scanf("%s", cmdBuffer);

        if (0 == strcmp(cmdBuffer, "exit"))
        {
            // exit command;
            g_bRun = false;
            printf("exit.\n");
            break ;
        }
        else if (0 == strcmp(cmdBuffer, "login"))
        {
            // send login command;
            Login login;
            strcpy(login.userName, "Steven Paul Jobs");
            strcpy(login.passWord, "redhat");
            if (SOCKET_ERROR == send(socket, (const char*)&login, sizeof(Login), 0))
            {
                printf("failed: send login error.\n");
                break;
            }
            else
            {
                printf("socket send login: CMD_LOGIN <cmd>, %d <bytes>, %s <username>, %s <password>.\n", login.length, login.userName, login.passWord);
            }
        }
        else if (0 == strcmp(cmdBuffer, "logout"))
        {
            // send logout command;
            Logout logout;
            strcpy(logout.userName, "Steven Paul Jobs");
            if (SOCKET_ERROR == send(socket, (const char*)&logout, sizeof(Logout), 0))
            {
                printf("failed: send logout error.\n");
                break;
            }
            else
            {
                printf("socket send logout: CMD_LOGOUT <cmd>, %d <bytes>, %s <username>.\n", logout.length, logout.userName);
            }
        }
        else
        {
            // unknown command;
            printf("error: unknown command.\n");
        }
    }
}

int main(int argc, char* argv[])
{
#ifdef _WIN32 // WIN32 socket programming environment support;
    WORD    vers = MAKEWORD(2, 2);
    WSADATA data = { 0 };  
    WSAStartup(vers, &data);
#endif

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
    serverAddr.sin_port = htons(9981);
#ifdef _WIN32
    serverAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
#else
    serverAddr.sin_addr.s_addr = inet_addr("172.168.1.1");
#endif
    if (SOCKET_ERROR == connect(serverSocket, (const sockaddr*)&serverAddr, sizeof(serverAddr)))
    {
        printf("failed: connect error.\n");
    }
    else {
        printf("socket connect successful.\n");
    }

    // user input command thread;
    std::thread thread(cmdTrhead, serverSocket);
    thread.detach();

    // 3. berkeley socket select programming;
    while (g_bRun)
    {
        fd_set fdsRead;
        FD_ZERO(&fdsRead);
        FD_SET(serverSocket, &fdsRead);
        timeval timeout = { 1, 0 };
        // 3.1 select system call.
        int fdCount = select(serverSocket + 1, &fdsRead, NULL, NULL, &timeout);
        if (SOCKET_ERROR == fdCount)
        {
            printf("failed: select error.\n");
            break;
        }
        // 3.2 receive message on a socket;
        if (FD_ISSET(serverSocket, &fdsRead))
        {
            FD_CLR(serverSocket, &fdsRead);
            if (-1 == processor(serverSocket))
            {
                printf("failed: processor error.\n");
                break;
            }
        }
        // 3.3 idle time work on other task(send message);
        // printf("idle time work on other tasks.\n");
    }

// 4. close a socket;
#ifdef _WIN32 // WIN32 socket programming environment support;
    closesocket(serverSocket);
    WSACleanup();
#else
    close(serverSocket);
#endif

    return 0;
}