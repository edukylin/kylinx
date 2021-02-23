#include "server.h"


// ���캯�� - ���������̻�����
TCPServer::TCPServer(): _listenSocket(INVALID_SOCKET)
{
	memset(_recvBuffer, 0, sizeof(_recvBuffer));
#ifdef _WIN32
	WORD    vers = MAKEWORD(2, 2);
	WSADATA data = { 0 };
	WSAStartup(vers, &data);
#endif
}

// �������� - �ͷ������̻�����
TCPServer::~TCPServer()
{
#ifdef _WIN32
	WSACleanup();
#endif
}

// �����׽��֣�
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

// ���׽��֣�
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

// �ر��׽��֣�
void TCPServer::close()
{
	// �رռ����׽��֣�
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
	// �ر�ͨ���׽��֣�
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

// �����׽��֣�
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

// ���տͻ��ˣ�
int TCPServer::accept()
{
	// �����¿ͻ�����������
	struct sockaddr_in addr = { 0 };
	socklen_t addrLength = sizeof(addr);
	SOCKET socket = ::accept(_listenSocket, (struct sockaddr*)&addr, (socklen_t*)&addrLength);
	if (INVALID_SOCKET == socket)
	{
		printf("failed: <%d> client_socket accept error.\n", socket);
		return INVALID_SOCKET;
	}
	printf("<%d> client_socket accept <%s> successful.\n", socket, inet_ntoa(addr.sin_addr));
	// Ⱥ���¿ͻ��˼�����Ϣ��
	NewUserJoin newUserJoin(socket);
	sendData2All(&newUserJoin);
	_clients.push_back(socket);
	return socket;
}

// ������Ϣ������
bool TCPServer::onMessage()
{
	if (!isValid())
	{
		return false;
	}
	fd_set  fdsRead, fdsWrite, fdsExcept;
	timeval timeout = { 0 };
	// ��ʼ����д�쳣�ļ�����������
	FD_ZERO(&fdsRead);
	FD_ZERO(&fdsWrite);
	FD_ZERO(&fdsExcept);
	// ��Ӽ����׽����ļ���������
	FD_SET(_listenSocket, &fdsRead);
	// ��������׽����ļ���������
	int maxSocket = _listenSocket;
	for (auto it = _clients.begin(); it != _clients.end(); it++)
	{
		maxSocket = (maxSocket > *it) ? maxSocket : *it;
		FD_SET(*it, &fdsRead);
	}
	// �첽������ģ�ͣ�
	int fdCount = select(maxSocket + 1, &fdsRead, &fdsWrite, &fdsExcept, NULL);
	if (SOCKET_ERROR == fdCount)
	{
		printf("failed: select error.\n");
		return false;
	}
	// �����׽�����Ӧ��������;
	if (FD_ISSET(_listenSocket, &fdsRead))
	{
		accept();
	}
	// ͨ���׽�����Ӧ��������;
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

// ��Ч�Լ�⣻
bool TCPServer::isValid()
{
	return INVALID_SOCKET != _listenSocket;
}

// �����������ݣ�
int TCPServer::recvData(SOCKET socket)
{
	DataHeader* pHeader = (DataHeader*)_recvBuffer;
	memset(_recvBuffer, 0, sizeof(_recvBuffer));
	// ������Ϣͷ��
	int headerCount = ::recv(socket, (char*)&_recvBuffer, sizeof(DataHeader), 0);
	if (headerCount <= 0)
	{
		(SOCKET_ERROR == headerCount) ? printf("failed: <%d> client_socket recv header error.\n", socket) : printf("failed: <%d> client_socket recv header close.\n", socket);
		return headerCount;
	}
	// ������Ϣ�壻
	int dataCount = ::recv(socket, (char*)&_recvBuffer+sizeof(DataHeader), pHeader->length-sizeof(DataHeader), 0);
	if (dataCount <= 0)
	{
		(SOCKET_ERROR == headerCount) ? printf("failed: <%d> client_socket recv data error.\n", socket) : printf("failed: <%d> client_socket recv data close.\n", socket);
		return headerCount;
	}

	return pHeader->length;
}

// �����������ݣ�
int TCPServer::sendData(SOCKET socket, DataHeader* pHeader)
{
	// �׽�����Ч��⣻
	if (INVALID_SOCKET == socket || pHeader == nullptr)
	{
		return SOCKET_ERROR;
	}
	// �׽��ַ������ݣ�
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

// Ⱥ���������ݣ�
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

// ������Ϣ����
void TCPServer::messageHandler(SOCKET socket)
{
	DataHeader* pHeader = (DataHeader*)_recvBuffer;
	switch (pHeader->command)
	{
		case CMD_LOGIN:
		{
			// ���յ�¼����;
			Login* pLogin = (Login*)_recvBuffer;
			printf("<%d> client_socket recv login data: CMD_LOGIN, <%d> bytes, <%s> username, <%s> password.\n", socket, pLogin->length, pLogin->userName, pLogin->passWord);
			// ���͵�¼��Ӧ;
			LoginResult loginResult;
			sendData(socket, &loginResult);
			break;
		}
		case CMD_LOGOUT:
		{
			// �����˳�����;
			Logout* pLogout = (Logout*)_recvBuffer;
			printf("<%d> client_socket recv login data: CMD_LOGOUT, <%d> bytes, <%s> username.\n", socket, pLogout->length, pLogout->userName);
			// �����˳���Ӧ;
			LogoutResult logoutResult;
			sendData(socket, &logoutResult);
			break;
		}
		default:
		{
			// ����δ֪����
			DataHeader* pHeader = (DataHeader*)_recvBuffer;
			printf("<%d> client_socket recv unknown header: <%d> command, <%d> bytes.\n", socket, pHeader->command, pHeader->length);
			// ����δ֪��Ӧ��
			DataHeader errorResult = { CMD_ERROR, 0 };
			sendData(socket, &errorResult);
			break;
		}
	}
}