#ifndef __TCP_CLIENT_H__
#define __TCP_CLIENT_H__

#ifdef _WIN32 // WIN32 header file, macro, and dll;
	#define WIN32_LEAN_AND_MEAN
	#define _WINSOCK_DEPRECATED_NO_WARNINGS
	#define _CRT_SECURE_NO_WARNINGS
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

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <thread>
#include "message.h"

class TCPClient
{
public:
	TCPClient();
	virtual ~TCPClient();
public:
	// �����׽��֣�
	void init();
	// ���ӷ�������
	void connect(const char* ip, const unsigned short port);
	// �ر��׽��֣�
	void close();
	// ��Ч�Լ�⣻
	bool isValid();
public:
	// ������Ϣ������
	bool onMessage();
	// ������Ϣ����
	void messageHandler();
	// �����������ݣ�
	int recvData();
	// �����������ݣ�
	int sendData(DataHeader* pHeader);
private:
	// �����׽��֣�
	SOCKET _socket;
	// ���ջ�������
	char _recvBuffer[1024];
};

#endif
