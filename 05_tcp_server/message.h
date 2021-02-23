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

// 用户登录数据包;
struct Login : public DataHeader
{
public:
    char userName[32];
    char passWord[32];
public:
    Login(const char* userName = "Steven Paul Jobs", const char* passWord = "redhat")
    {
        command = CMD_LOGIN;
        length = sizeof(Login);
        memset(this->userName, 0, sizeof(this->userName));
        memset(this->passWord, 0, sizeof(this->passWord));
        strcpy(this->userName, userName);
        strcpy(this->passWord, passWord);
    }
};

// 登录结果数据包;
struct LoginResult : public DataHeader
{
public:
    int result;
public:
    LoginResult(int result = 0)
    {
        command = CMD_LOGIN_RESULT;
        length = sizeof(LoginResult);
        this->result = result; // correct;
    }
};

// 用户退出数据包；
struct Logout : public DataHeader
{
public:
    char userName[32];
public:
    Logout(const char* userName = "Steven Paul Jobs")
    {
        command = CMD_LOGOUT;
        length = sizeof(Logout);
        memset(this->userName, 0, sizeof(this->userName));
        strcpy(this->userName, userName);
    }
};

// 退出结果数据包；
struct LogoutResult : public DataHeader
{
public:
    int  result;
public:
    LogoutResult(int result = 0)
    {
        command = CMD_LOGOUT_RESULT;
        length = sizeof(LogoutResult);
        this->result = 0; // correct;
    }
};

// 用户加入数据包;
struct NewUserJoin : public DataHeader
{
public:
    int  socket;
public:
    NewUserJoin(int socket = 0)
    {
        command = CMD_NEW_USER_JOIN;
        length = sizeof(NewUserJoin);
        this->socket = socket; // correct;
    }
};

#endif
