#include "server.h"


// 构造函数 - 创建网络编程环境；
TCPServer::TCPServer(): _listenSocket(INVALID_SOCKET)
{
	memset(_recvBuffer, 0, sizeof(_recvBuffer));
#ifdef _WIN32
	WORD    vers = MAKEWORD(2, 2);
	WSADATA data = { 0 };
	WSAStartup(vers, &data);
#endif
}

// 析构函数 - 释放网络编程环境；
TCPServer::~TCPServer()
{
#ifdef _WIN32
	WSACleanup();
#endif
}

// 创建套接字；
void TCPServer::init()
{
	if (INVALID_SOCKET != _listenSocket)
	{
		close();
	}
	_listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == _listenSocket)
	{
		printf("failed: <%d> listen_socket create error.\n", _listenSocket);
	}
	else
	{
		printf("<%d> listen_socket create successful.\n", _listenSocket);
	}
}

// 绑定套接字；
int TCPServer::bind(const char* ip, const unsigned short port)
{
	if (INVALID_SOCKET == _listenSocket)
	{
		init();
	}
	struct sockaddr_in addr = { 0 };
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
#ifdef _WIN32
	addr.sin_addr.S_un.S_addr = ip ? inet_addr(ip): INADDR_ANY;
#else
	addr.sin_addr.s_addr = ip ? inet_addr(ip) : INADDR_ANY;
#endif
	int res = ::bind(_listenSocket, (const sockaddr*)&addr, sizeof(addr));
	if (SOCKET_ERROR == res)
	{
		printf("failed: <%d> listen_socket bind error.\n", _listenSocket);
		return SOCKET_ERROR;
	}
	else {
		printf("<%d> listen_socket bind successful.\n", _listenSocket);
	}
	return res;
}

// 关闭套接字；
void TCPServer::close()
{
	// 关闭监听套接字；
	if (INVALID_SOCKET != _listenSocket)
	{
#ifdef _WIN32
		closesocket(_listenSocket);
#else
		::close(_listenSocket);
#endif
		printf("<%d> listen_socket close successful.\n", _listenSocket);
		_listenSocket = INVALID_SOCKET;
	}
	// 关闭通信套接字；
	for (auto it = _clients.begin(); it != _clients.end(); it++)
	{
		if (*it != INVALID_SOCKET) {
#ifdef _WIN32
			closesocket(*it);
#else
			::close(*it);
#endif
		}
	}
	_clients.clear();
}

// 监听套接字；
int TCPServer::listen(int backlog)
{
	if (INVALID_SOCKET == _listenSocket)
	{
		bind();
	}
	int res = ::listen(_listenSocket, backlog);
	if (SOCKET_ERROR == res)
	{
		printf("failed: <%d> listen_socket listen error.\n", _listenSocket);
		return SOCKET_ERROR;
	}
	else
	{
		printf("<%d> listen_socket listen successful.\n", _listenSocket);
	}
	return res;
}

// 接收客户端；
int TCPServer::accept()
{
	// 接受新客户端连接请求；
	struct sockaddr_in addr = { 0 };
	socklen_t addrLength = sizeof(addr);
	SOCKET socket = ::accept(_listenSocket, (struct sockaddr*)&addr, (socklen_t*)&addrLength);
	if (INVALID_SOCKET == socket)
	{
		printf("failed: <%d> client_socket accept error.\n", socket);
		return INVALID_SOCKET;
	}
	printf("<%d> client_socket accept <%s> successful.\n", socket, inet_ntoa(addr.sin_addr));
	// 群发新客户端加入消息；
	NewUserJoin newUserJoin(socket);
	sendData2All(&newUserJoin);
	_clients.push_back(socket);
	return socket;
}

// 网络消息监听；
bool TCPServer::onMessage()
{
	if (!isValid())
	{
		return false;
	}
	fd_set  fdsRead, fdsWrite, fdsExcept;
	timeval timeout = { 0 };
	// 初始化读写异常文件描述符集；
	FD_ZERO(&fdsRead);
	FD_ZERO(&fdsWrite);
	FD_ZERO(&fdsExcept);
	// 添加监听套接字文件描述符；
	FD_SET(_listenSocket, &fdsRead);
	// 计算最大套接字文件描述符；
	int maxSocket = _listenSocket;
	for (auto it = _clients.begin(); it != _clients.end(); it++)
	{
		maxSocket = (maxSocket > *it) ? maxSocket : *it;
		FD_SET(*it, &fdsRead);
	}
	// 异步网络编程模型；
	int fdCount = select(maxSocket + 1, &fdsRead, &fdsWrite, &fdsExcept, NULL);
	if (SOCKET_ERROR == fdCount)
	{
		printf("failed: select error.\n");
		return false;
	}
	// 监听套接字响应网络请求;
	if (FD_ISSET(_listenSocket, &fdsRead))
	{
		accept();
	}
	// 通信套接字响应网络请求;
	for (auto it = _clients.begin(); it != _clients.end();)
	{
		if (FD_ISSET(*it, &fdsRead))
		{
			if (recvData(*it) <= 0)
			{
				it = _clients.erase(it);
				continue;
			}
			messageHandler(*it);
		}
		it++;
	}
	return true;
}

// 有效性检测；
bool TCPServer::isValid()
{
	return INVALID_SOCKET != _listenSocket;
}

// 接收网络数据；
int TCPServer::recvData(SOCKET socket)
{
	DataHeader* pHeader = (DataHeader*)_recvBuffer;
	memset(_recvBuffer, 0, sizeof(_recvBuffer));
	// 接收消息头；
	int headerCount = ::recv(socket, (char*)&_recvBuffer, sizeof(DataHeader), 0);
	if (headerCount <= 0)
	{
		(SOCKET_ERROR == headerCount) ? printf("failed: <%d> client_socket recv header error.\n", socket) : printf("failed: <%d> client_socket recv header close.\n", socket);
		return headerCount;
	}
	// 接收消息体；
	int dataCount = ::recv(socket, (char*)&_recvBuffer+sizeof(DataHeader), pHeader->length-sizeof(DataHeader), 0);
	if (dataCount <= 0)
	{
		(SOCKET_ERROR == headerCount) ? printf("failed: <%d> client_socket recv data error.\n", socket) : printf("failed: <%d> client_socket recv data close.\n", socket);
		return headerCount;
	}

	return pHeader->length;
}

// 发送网络数据；
int TCPServer::sendData(SOCKET socket, DataHeader* pHeader)
{
	// 套接字有效检测；
	if (INVALID_SOCKET == socket || pHeader == nullptr)
	{
		return SOCKET_ERROR;
	}
	// 套接字发送数据；
	int sendCount = ::send(socket, (const char*)pHeader, pHeader->length, 0);
	if (SOCKET_ERROR == sendCount)
	{
		printf("failed: <%d> client_socket send <%d> command error.\n", socket, pHeader->command);
		return SOCKET_ERROR;
	}
	else
	{
		printf("<%d> client_socket send <%d> command, <%d> bytes.\n", socket, pHeader->command, pHeader->length);
	}
	return sendCount;
}

// 群发网络数据；
int TCPServer::sendData2All(DataHeader* pHeader)
{
	int clientCount = 0;
	for (auto it = _clients.begin(); it != _clients.end(); it++)
	{
		int sendCount = sendData(*it, pHeader);
		if (SOCKET_ERROR != sendCount)
		{
			clientCount += 1;
		}	
	}
	return clientCount;
}

// 网络消息处理；
void TCPServer::messageHandler(SOCKET socket)
{
	DataHeader* pHeader = (DataHeader*)_recvBuffer;
	switch (pHeader->command)
	{
		case CMD_LOGIN:
		{
			// 接收登录请求;
			Login* pLogin = (Login*)_recvBuffer;
			printf("<%d> client_socket recv login data: CMD_LOGIN, <%d> bytes, <%s> username, <%s> password.\n", socket, pLogin->length, pLogin->userName, pLogin->passWord);
			// 发送登录响应;
			LoginResult loginResult;
			sendData(socket, &loginResult);
			break;
		}
		case CMD_LOGOUT:
		{
			// 接收退出请求;
			Logout* pLogout = (Logout*)_recvBuffer;
			printf("<%d> client_socket recv login data: CMD_LOGOUT, <%d> bytes, <%s> username.\n", socket, pLogout->length, pLogout->userName);
			// 发送退出响应;
			LogoutResult logoutResult;
			sendData(socket, &logoutResult);
			break;
		}
		default:
		{
			// 接收未知请求；
			DataHeader* pHeader = (DataHeader*)_recvBuffer;
			printf("<%d> client_socket recv unknown header: <%d> command, <%d> bytes.\n", socket, pHeader->command, pHeader->length);
			// 发送未知响应；
			DataHeader errorResult = { CMD_ERROR, 0 };
			sendData(socket, &errorResult);
			break;
		}
	}
}