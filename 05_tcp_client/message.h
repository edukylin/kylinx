#ifndef __MESSAGE_H__
#define __MESSAGE_H__

#include <cstring>

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
        strcpy(userName, "Steven Paul Jobs");
        strcpy(passWord, "redhat");
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
        strcpy(userName, "Steven Paul Jobs");
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

#endif
