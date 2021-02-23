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

// �û���¼���ݰ�;
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

// ��¼������ݰ�;
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

// �û��˳����ݰ���
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

// �˳�������ݰ���
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

// �û��������ݰ�;
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
