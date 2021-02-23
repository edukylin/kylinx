#include "client.h"

// 构造函数 - 创建网络编程环境；
TCPClient::TCPClient() :_socket(INVALID_SOCKET)
{
	memset(_recvBuffer, 0, sizeof(_recvBuffer));
#ifdef _WIN32 // WIN32 socket programming environment support;
	WORD    vers = MAKEWORD(2, 2);
	WSADATA data = { 0 };
	WSAStartup(vers, &data);
#endif
}

// 析构函数 - 释放网络编程环境；
TCPClient::~TCPClient()
{
#ifdef _WIN32 // WIN32 socket programming environment support;
	WSACleanup();
#endif
}

// 创建套接字；
void TCPClient::init()
{
	if (INVALID_SOCKET != _socket)
	{
		close();
	}
	_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == _socket)
	{
		printf("failed: <%d> socket create error.\n", _socket);
	}
	else
	{
		printf("<%d> socket create successful.\n", _socket);
	}
}

// 连接服务器；
void TCPClient::connect(const char* ip, const unsigned short port)
{
	if (INVALID_SOCKET == _socket)
	{
		init();
	}
	struct sockaddr_in addr = { 0 };
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
#ifdef _WIN32
	addr.sin_addr.S_un.S_addr = inet_addr(ip);
#else
	addr.sin_addr.s_addr = inet_addr(ip);
#endif
	if (SOCKET_ERROR == ::connect(_socket, (const sockaddr*)&addr, sizeof(addr)))
	{
		printf("failed: <%d> socket connect error.\n", _socket);
	}
	else {
		printf("<%d> socket connect successful.\n", _socket);
	}
}

// 关闭套接字；
void TCPClient::close()
{
#ifdef _WIN32
	closesocket(_socket);
#else
	::close(_socket);
#endif
	printf("<%d> socket close successful.\n", _socket);
	_socket = INVALID_SOCKET;
}

// 有效性检测；
bool TCPClient::isValid()
{
	return INVALID_SOCKET != _socket;
}

// 网络消息监听；
bool TCPClient::onMessage()
{
	if (!isValid())
	{
		return false;
	}
	fd_set fdsRead;
	FD_ZERO(&fdsRead);
	FD_SET(_socket, &fdsRead);
	timeval timeout = { 1, 0 };
	int fdCount = select(_socket + 1, &fdsRead, NULL, NULL, &timeout);
	if (SOCKET_ERROR == fdCount)
	{
		printf("failed: select error.\n");
		return false;
	}
	if (FD_ISSET(_socket, &fdsRead))
	{
		FD_CLR(_socket, &fdsRead);
		if (SOCKET_ERROR == recvData())
		{
			return false;
		}
		messageHandler(); // 网络消息处理；
	}
	return true;
}

// 网络消息处理；
void TCPClient::messageHandler()
{
	DataHeader* pHeader = (DataHeader*)_recvBuffer;
	switch (pHeader->command)
	{
		case CMD_LOGIN_RESULT:
		{
			LoginResult* pLoginResult = (LoginResult*)_recvBuffer;
			printf("<%d> socket recv login result data: CMD_LOGIN_RESULT, <%d> bytes, <%d> result.\n", _socket, pLoginResult->length, pLoginResult->result);
			break;
		}
		case CMD_LOGOUT_RESULT:
		{
			LogoutResult* pLogoutResult = (LogoutResult*)_recvBuffer;
			printf("<%d> socket recv logout result data: CMD_LOGOUT_RESULT, <%d> bytes, <%d> result.\n", _socket, pLogoutResult->length, pLogoutResult->result);
			break;
		}
		case CMD_NEW_USER_JOIN:
		{
			NewUserJoin* pNewUserJoin = (NewUserJoin*)_recvBuffer;
			printf("<%d> socket recv new user join data: CMD_NEW_USER_JOIN, <%d> bytes, <%d> id.\n", _socket, pNewUserJoin->length, pNewUserJoin->socket);
			break;
		}
		default:
		{
			printf("<%d> socket recv unknown header: <%d> cmd, <%d> bytes.\n", _socket, pHeader->command, pHeader->length);
			break;
		}
	}
}

// 发送网络数据；
int TCPClient::sendData(DataHeader* pHeader)
{
	if (!isValid() || !pHeader)
	{
		return SOCKET_ERROR;
	}
	int dataCount = send(_socket, (const char*)pHeader, pHeader->length, 0);
	if (SOCKET_ERROR == dataCount)
	{
		printf("failed: <%d> socket send error.\n", _socket);
	}
	else
	{
		printf("<%d> socket send data: <%d> cmd, <%d> bytes.\n", _socket, pHeader->command, pHeader->length);
	}
	return dataCount;
}

// 接收网络数据；
int TCPClient::recvData()
{
	DataHeader* pHeader = (DataHeader*)_recvBuffer;
	memset(_recvBuffer, 0, sizeof(_recvBuffer));
	int headerCount = recv(_socket, (char*)&_recvBuffer, sizeof(DataHeader), 0);
	if (headerCount <= 0)
	{
		_socket = INVALID_SOCKET;
		(SOCKET_ERROR == headerCount) ? printf("failed: <%d> socket recv header error.\n", _socket) : printf("failed: <%d> socket recv header close.\n", _socket);
		return SOCKET_ERROR;
	}
	int dataCount = recv(_socket, _recvBuffer + sizeof(DataHeader), pHeader->length - sizeof(DataHeader), 0);
	if (dataCount <= 0)
	{
		(SOCKET_ERROR == dataCount) ? printf("failed: <%d> socket recv data error.\n", _socket) : printf("failed: <%d> socket recv data close.\n", _socket);
		return SOCKET_ERROR;
	}
	return pHeader->length;
}
