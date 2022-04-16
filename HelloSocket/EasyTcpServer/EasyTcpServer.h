#pragma once
#ifndef EASY_TCP_SERVER
#define EASY_TCP_SERVER


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
#include <vector>
#include "MessageHeader.h"

class EasyTcpServer {
private:
	SOCKET _sock;

	std::vector<SOCKET> g_clients;
public:
	EasyTcpServer() {
		_sock = INVALID_SOCKET;
	}


	//��ʼ��socket
	SOCKET initSocket() {
#ifdef _WIN32
		WORD ver = MAKEWORD(2, 2);
		WSAData dat;
		WSAStartup(ver, &dat);
#endif
		if (INVALID_SOCKET != _sock) {
			printf("�رվ�����...");
			//close();
		}
		_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (INVALID_SOCKET == _sock) {
			printf("���󣬽���sockeʧ��..");
		}
		else {
			printf("����socket �ɹ�");
		}

		return _sock;


	}
	//�󶨶˿ں�
	int bindSocket(unsigned short port) {
		sockaddr_in _sin = {};
		_sin.sin_family = AF_INET;
		_sin.sin_port = htons(port);
#ifdef _WIN32
		_sin.sin_addr.S_un.S_addr = INADDR_ANY;
#else
		_sin.sin_addr.s_addr = INADDR_ANY;
#endif
		if (SOCKET_ERROR == bind(_sock, (sockaddr*)&_sin, sizeof(_sin))) {
			printf("���󣬰�����˿�ʧ��...\n");
		}
		else {
			printf("������˿ڳɹ�... \n");
		}
		return 0;
	}
	//�����˿ں�
	int listenSocket(int n) {
		int ret = listen(_sock, n);
		if (SOCKET_ERROR == ret) {
			printf("���󣬼�������˿�ʧ��... \n");
		}
		else {
			printf("��������˿ڳɹ�...\n");
		}

		return ret;
	}
	//���տͻ���
	int acceptSocket() {
		sockaddr_in clientAddr = {};
		int nAddrLen = sizeof(sockaddr_in);
		SOCKET _cSock = INVALID_SOCKET;
#ifdef _WIN32
		_cSock = accept(_sock, (sockaddr *)&clientAddr, &nAddrLen);
#else
		_cSock = accept(_sock, (sockaddr *)&clientAddr, (socklen_t  *)&nAddrLen);
#endif
		if (INVALID_SOCKET == _cSock) {
			printf("���󣬽��յ���Ч�ͻ���SOCKET...\n");
		} 
		else {
			NewUserJoin userJoin;
			userJoin.sock = _cSock;
			sendDataToAll(&userJoin);
			printf("�¿ͻ��˼��룺Socket = %d, IP = %s \n", (int)_cSock, inet_ntoa(clientAddr.sin_addr));
			g_clients.push_back(_cSock);
		}

		return  _cSock;
	
	}
	//�ر�socket

	void close() {
		if (_sock != INVALID_SOCKET) {
#ifdef _WIN32
			for (int n = g_clients.size() - 1; n >= 0; n--) {
				closesocket(g_clients[n]);
			}
			closesocket(_sock);
			WSACleanup();
#else
			close(_sock);
#endif
			_sock = INVALID_SOCKET;
		}
	}


	//����������Ϣ 
	bool onRun() {
		if (isRun()) {
			fd_set fdRead;
			fd_set fdWrite;
			fd_set fdExp;
			FD_ZERO(&fdRead);
			FD_ZERO(&fdWrite);
			FD_ZERO(&fdExp);
			FD_SET(_sock, &fdRead);
			FD_SET(_sock, &fdWrite);
			FD_SET(_sock, &fdExp);


			for (int n = (int)g_clients.size() - 1; n >= 0; n--) {
				FD_SET(g_clients[n], &fdRead);
			}

			timeval t = { 0, 0 };
			int ret = select(_sock + 1, &fdRead, &fdWrite, &fdExp, &t);
			if (ret < 0) {
				//����
				printf("select ������� \n");
				return false;
			}



			if (FD_ISSET(_sock, &fdRead)) {
				FD_CLR(_sock, &fdRead);
				//4.accept �ȴ����ܿͻ�������
				acceptSocket();
			}

			for (size_t n = 0; n < fdRead.fd_count; n++) {
				if (-1 == recvData(fdRead.fd_array[n])) {
					auto iter = find(g_clients.begin(), g_clients.end(), fdRead.fd_array[n]);
					if (iter != g_clients.end()) {
						g_clients.erase(iter);
					}
				}
			}

			return true;
		}
		else {
			return false;
		}
	}
	//�Ƿ�����
	bool isRun() {
		return _sock != INVALID_SOCKET;
	}
	//�������� ����ճ�� ��ְ�
	int recvData(SOCKET _cSocket) {
		char szRecv[1024] = {};

		int nLen = recv(_cSocket, szRecv, sizeof(DataHeader), 0);
		if (nLen <= 0)
		{
			printf("�ͻ������˳����������");
			return -1;
		}

		DataHeader* header = (DataHeader*)szRecv;
		recv(_cSocket, szRecv + sizeof(DataHeader), header->dataLenth - sizeof(DataHeader), 0);
		onNetMsg(_cSocket, header);

		
	}

	//��Ӧ������Ϣ
	virtual void onNetMsg(SOCKET _cSocket, DataHeader * header) {
		switch (header->cmd) {
		case CMD_LOGIN: {
			
			Login* login = reinterpret_cast<Login *>(header);
			printf("���ܵ�CMD_LOGIN  ���ݳ���:%d  userName:%s  passworkd:%s  \n", login->dataLenth, login->userName, login->passWord);
			//�����ж��û��������Ƿ���ȷ�Ĺ���
			LoginResult resutIn;
			sendData(_cSocket, &resutIn);
			break;
		}
		case CMD_LOGOUT: {
		
			LogOut* loginOut = reinterpret_cast<LogOut *>(header);
			printf("���ܵ�CMD_LOGOUT ���ݳ���:%d  userName:%s  \n", loginOut->dataLenth, loginOut->userName);
			LogOutResult resultOut;
			sendData(_cSocket, &resultOut);
			break;
		}
		default: {
			DataHeader header;
			header.cmd = CMD_ERROR;
			header.dataLenth = sizeof(DataHeader);
			sendData(_cSocket, &header);
			break;
		}
		}
	}

	//����ָ������
	int sendData(SOCKET _cSocket, DataHeader* header) {
		if (isRun() && header) {
			return send(_cSocket, (const char *)header, header->dataLenth, 0);
		}
		return SOCKET_ERROR;
	}


	void sendDataToAll(DataHeader* header) {
		if (isRun() && header) {
			for (int n = (int)g_clients.size() - 1; n >= 0; n--) {
				sendData(g_clients[n], header);
				//send(g_clients[n], (const char*)header, header->dataLenth, 0);
			}
		}
	}



	virtual ~EasyTcpServer() {
		close();
	}

};




#endif