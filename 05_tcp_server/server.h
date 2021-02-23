#ifndef __TCP_SERVER_H__
#define __TCP_SERVER_H__

#ifdef _WIN32 // WIN32 header file, macro, and dll;
	#define WIN32_LEAN_AND_MEAN
	#define _WINSOCK_DEPRECATED_NO_WARNINGS
	#define _CRT_SECURE_NO_WARNINGS
	#include <Windows.h>
	#include <WinSock2.h>
	#pragma comment(lib, "ws2_32.lib")
	typedef int socklen_t;
#else
	#include <unistd.h>
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <arpa/inet.h>
	typedef int SOCKET;
	#define SOCKET_ERROR    (-1)
	#define INVALID_SOCKET  (SOCKET)(~0)
#endif

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <thread>
#include <vector>
#include "message.h"

class TCPServer
{
public:
	TCPServer();
	virtual ~TCPServer();
public:
	// �����׽��֣�
	void init();
	// ���׽��֣�
	int bind(const char* ip = nullptr, const unsigned short port = 9981);
	// �ر��׽��֣�
	void close();
	// �����׽��֣�
	int listen(int backlog = 5);
	// ���տͻ���;
	int accept();
	// ��Ч�Լ�⣻
	bool isValid();
	// ������Ϣ������
	bool onMessage();
	// �����������ݣ�
	int recvData(SOCKET socket = INVALID_SOCKET);
	// �����������ݣ�
	int sendData(SOCKET socket = INVALID_SOCKET, DataHeader* pHeader = nullptr);
	// Ⱥ���������ݣ�
	int sendData2All(DataHeader* pHeader);
public:
	// ������Ϣ����
	virtual void messageHandler(SOCKET socket);// Pheader
private:
	SOCKET	_listenSocket;
	std::vector<SOCKET>	_clients;
	char	_recvBuffer[1024];
};

#endif

