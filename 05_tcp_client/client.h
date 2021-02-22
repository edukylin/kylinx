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
	// 创建套接字；
	void init();
	// 连接服务器；
	void connect(const char* ip, const unsigned short port);
	// 关闭套接字；
	void close();
	// 有效性检测；
	bool isValid();
public:
	// 网络消息监听；
	bool onMessage();
	// 网络消息处理；
	void messageHandler();
	// 接收网络数据；
	int recvData();
	// 发送网络数据；
	int sendData(DataHeader* pHeader);
private:
	// 网络套接字；
	SOCKET _socket;
	// 接收缓冲区；
	char _recvBuffer[1024];
};

#endif
