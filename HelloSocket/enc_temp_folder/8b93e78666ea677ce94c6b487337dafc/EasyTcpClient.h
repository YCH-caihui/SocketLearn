#ifndef EASY_TCP_CLIENT
#define EASY_TCP_CLIENT

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <WinSock2.h>
#pragma comment(lib,"ws2_32.lib")
#else
#include<unistd.h>
#include<arpa/inet.h>
#include<string.h>

#define SOCKET int
#define INVALID_SOCKET (SOCKET)(~0)
#define SOCKET_ERROE           (-1)
#endif

#include <stdio.h>
#include "MessageHeader.h"

class EasyTcpClient {

	SOCKET _sock;

public:
	EasyTcpClient() {
		_sock = INVALID_SOCKET;
	}


	virtual ~EasyTcpClient() {
		close();
	}


	//初始化socket
	int initSocket() {
		//启动 socket
#ifdef _WIN32 
		WORD ver = MAKEWORD(2, 2);
		WSADATA dat;
		WSAStartup(ver, &dat);
#endif
		if (INVALID_SOCKET != _sock) {
			printf("已经连接了，关闭旧的连接");
			close();
		}
		_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (_sock == INVALID_SOCKET) {
			printf("错误，建立socket失败...\n");
		}
		else {
			printf("建立socket成功..\n");
		}

		return 0;
	}


	//连接服务器
	int connectSocket(const char* ip, unsigned short port) {

		if (INVALID_SOCKET == _sock) {
			initSocket();
		}

		sockaddr_in _sin = {};
		_sin.sin_family = AF_INET;
		_sin.sin_port = htons(port);
#ifdef _WIN32
		_sin.sin_addr.S_un.S_addr = inet_addr(ip);
#else 
		_sin.sin_addr.s_addr = inet_addr(ip);
#endif
		int ret = connect(_sock, (sockaddr*)&_sin, sizeof(sockaddr_in));
		if (ret == SOCKET_ERROR) {
			printf("错误，连接服务器事变...\n");
		}
		else {
			printf("连接服务器成功... \n");
		}

		return ret;
	}

	//关闭socket

	void close() {
		if (_sock != INVALID_SOCKET) {
#ifdef _WIN32
			closesocket(_sock);
			WSACleanup();
#else
			close(_sock);
#endif
			_sock = INVALID_SOCKET;
		}
	}

	//查询网络消息
	bool onRun() {
		if (isRun()) {
			fd_set fdReads;
			FD_ZERO(&fdReads);
			FD_SET(_sock, &fdReads);
			timeval t = { 1, 0 };
			int ret = select(_sock + 1, &fdReads, 0, 0, &t);
			if (ret < 0) {
				printf("select 任务结束  \n");
				return false;
			}
			if (FD_ISSET(_sock, &fdReads)) {
				FD_CLR(_sock, &fdReads);

				if (-1 == recvData(_sock)) {
					printf("<socket==%d>select任务结束2 \n", _sock);
					return false;
				}
			}

			return true;
		}
		return false;
	}

	//是否工作中
	bool isRun() {
		return _sock != INVALID_SOCKET;
	}

	//接受数据
	int recvData(SOCKET _cSocket) {
		char szRecv[4096] = {};

		int nLen = recv(_cSocket, szRecv, sizeof(DataHeader), 0);
		if (nLen <= 0)
		{
			printf("与服务器断开链接,任务结束");
			return -1;
		}

		DataHeader* header = (DataHeader*)szRecv;
		recv(_cSocket, szRecv + sizeof(DataHeader), header->dataLenth - sizeof(DataHeader), 0);
		onNetMsg(header);

	}

	//接收数据 
	void onNetMsg(DataHeader * header) {


		switch (header->cmd) {
		case CMD_LOGIN_RESULT: {
		
			LoginResult* login = (LoginResult*)&header;
			printf("接受到CMD_LOGIN_RESULT  数据长度:%d  Result:%d   \n", login->dataLenth, login->result);

			break;
		}
		case CMD_LOGOUT_RESULT: {
			LogOutResult* login = (LogOutResult*)&header;
			printf("接受到CMD_LOGOUT_RESULT  数据长度:%d  Result:%d   \n", login->dataLenth, login->result);
			break;
		}
		case CMD_NEW_USER_JOIN: {
			NewUserJoin* login = (NewUserJoin*)&header;
			printf("接受到CMD_NEW_USER_JOIN  数据长度:%d  sock:%d   \n", login->dataLenth, login->sock);
			break;
		}

		default: {
			DataHeader header;
			header.cmd = CMD_ERROR;
			header.dataLenth = sizeof(DataHeader);
			send(_sock, (char*)&header, sizeof(DataHeader), 0);
			break;
		}
		}
	}

	//发送数据
    int	sendData(DataHeader * header) {
		if (isRun() && header) {
		  return send(_sock, (const char*)header, header->dataLenth, 0);
		}
		return SOCKET_ERROR;
	}

private:

};

#endif