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


	//��ʼ��socket
	int initSocket() {
		//���� socket
#ifdef _WIN32 
		WORD ver = MAKEWORD(2, 2);
		WSADATA dat;
		WSAStartup(ver, &dat);
#endif
		if (INVALID_SOCKET != _sock) {
			printf("�Ѿ������ˣ��رվɵ�����");
			close();
		}
		_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (_sock == INVALID_SOCKET) {
			printf("���󣬽���socketʧ��...\n");
		}
		else {
			printf("����socket�ɹ�..\n");
		}

		return 0;
	}


	//���ӷ�����
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
			printf("�������ӷ������±�...\n");
		}
		else {
			printf("���ӷ������ɹ�... \n");
		}

		return ret;
	}

	//�ر�socket

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

	//��ѯ������Ϣ
	bool onRun() {
		if (isRun()) {
			fd_set fdReads;
			FD_ZERO(&fdReads);
			FD_SET(_sock, &fdReads);
			timeval t = { 1, 0 };
			int ret = select(_sock + 1, &fdReads, 0, 0, &t);
			if (ret < 0) {
				printf("select �������  \n");
				return false;
			}
			if (FD_ISSET(_sock, &fdReads)) {
				FD_CLR(_sock, &fdReads);

				if (-1 == recvData(_sock)) {
					printf("<socket==%d>select�������2 \n", _sock);
					return false;
				}
			}

			return true;
		}
		return false;
	}

	//�Ƿ�����
	bool isRun() {
		return _sock != INVALID_SOCKET;
	}

	//��������
	int recvData(SOCKET _cSocket) {
		char szRecv[4096] = {};

		int nLen = recv(_cSocket, szRecv, sizeof(DataHeader), 0);
		if (nLen <= 0)
		{
			printf("��������Ͽ�����,�������");
			return -1;
		}

		DataHeader* header = (DataHeader*)szRecv;
		recv(_cSocket, szRecv + sizeof(DataHeader), header->dataLenth - sizeof(DataHeader), 0);
		onNetMsg(header);

	}

	//�������� 
	void onNetMsg(DataHeader * header) {


		switch (header->cmd) {
		case CMD_LOGIN_RESULT: {
		
			LoginResult* login = (LoginResult*)&header;
			printf("���ܵ�CMD_LOGIN_RESULT  ���ݳ���:%d  Result:%d   \n", login->dataLenth, login->result);

			break;
		}
		case CMD_LOGOUT_RESULT: {
			LogOutResult* login = (LogOutResult*)&header;
			printf("���ܵ�CMD_LOGOUT_RESULT  ���ݳ���:%d  Result:%d   \n", login->dataLenth, login->result);
			break;
		}
		case CMD_NEW_USER_JOIN: {
			NewUserJoin* login = (NewUserJoin*)&header;
			printf("���ܵ�CMD_NEW_USER_JOIN  ���ݳ���:%d  sock:%d   \n", login->dataLenth, login->sock);
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

	//��������
    int	sendData(DataHeader * header) {
		if (isRun() && header) {
		  return send(_sock, (const char*)header, header->dataLenth, 0);
		}
		return SOCKET_ERROR;
	}

private:

};

#endif