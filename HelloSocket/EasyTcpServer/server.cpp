#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include <windows.h>
#include <WinSock2.h>
#include <stdio.h>
#include <vector>


enum CMD {
	CMD_LOGIN, 
	CMD_LOGIN_RESULT,
	CMD_LOGOUT,
	CMD_LOGOUT_RESULT,
	CMD_ERROR
};

//��Ϣͷ
struct DataHeader {
	short dataLenth;
	short cmd;
};

//��Ϣ��
struct Login : public DataHeader {
	char userName[32];
	char passWord[32];

	Login() {
		dataLenth = sizeof(Login);
		cmd = CMD_LOGIN;
	}
};


struct LoginResult : public DataHeader {
	int result;
	
	LoginResult() {
		dataLenth = sizeof(LoginResult);
		cmd = CMD_LOGIN_RESULT;
		result = 0;
	}
};

struct LogOut : public DataHeader {
	char userName[32];

	LogOut() {
		dataLenth = sizeof(LogOut);
		cmd = CMD_LOGOUT;
	}
};

struct LogOutResult : public DataHeader {
	int result;

	LogOutResult() {
		dataLenth = sizeof(LogOutResult);
		cmd = CMD_LOGOUT_RESULT;
		result = 0;
	}
};


std::vector<SOCKET> g_clients;




int processor(SOCKET _cSocket) {
	char szRecv[1024] = {};

	int nLen = recv(_cSocket, szRecv, sizeof(DataHeader), 0);
	if (nLen <= 0)
	{
		printf("�ͻ������˳����������");
		return -1;
	}

	DataHeader* header = (DataHeader*)szRecv;


	switch (header->cmd) {
	case CMD_LOGIN: {
		recv(_cSocket, szRecv + sizeof(DataHeader), sizeof(Login) - sizeof(DataHeader), 0);
		Login* login = (Login*)&szRecv;
		printf("���ܵ�CMD_LOGIN  ���ݳ���:%d  userName:%s  passworkd:%s  \n", login->dataLenth, login->userName, login->passWord);
		//�����ж��û��������Ƿ���ȷ�Ĺ���
		LoginResult resutIn;
		send(_cSocket, (char*)&resutIn, sizeof(LoginResult), 0);
		break;
	}
	case CMD_LOGOUT: {
		recv(_cSocket, szRecv + sizeof(DataHeader), sizeof(LogOut) - sizeof(DataHeader), 0);
		LogOut* loginOut = (LogOut*)szRecv;
		printf("���ܵ�CMD_LOGOUT ���ݳ���:%d  userName:%s  \n", loginOut->dataLenth, loginOut->userName);
		LogOutResult resultOut;
		send(_cSocket, (char*)&resultOut, sizeof(LogOutResult), 0);
		break;
	}
	default: {
		DataHeader header;
		header.cmd = CMD_ERROR;
		header.dataLenth = sizeof(DataHeader);
		send(_cSocket, (char*)&header, sizeof(DataHeader), 0);
		break;
	}
	}
}


int  main() {

	//windows ����socket����
	WORD ver = MAKEWORD(2, 2);
	WSADATA data;
	WSAStartup(ver, &data);

	//��socket API��������TCP ������
	//1.����һ��socket �׽���
 	SOCKET _sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	//2.bind �����ڽ��ܿͻ������ӵ�����˿�
	sockaddr_in _sin = {};
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(4567);
	_sin.sin_addr.S_un.S_addr = INADDR_ANY; //inet_addr("127.0.0.1");
	if (bind(_sock, (sockaddr*)&_sin, sizeof(_sin)) == SOCKET_ERROR) {
		printf("error,�����ڽ��ܿͻ������ӵ�����˿�ʧ��\n");
		return -1;
	}
	else {
		printf("������˿ڳɹ���\n");
	}
	//3.listen��������˿�
	if (listen(_sock, 5) == SOCKET_ERROR) {
		printf("���󣬼�������˿�ʧ��  \n");
		return -2;
	}
	else {
		printf("��������˿ڳɹ�   \n");
	}
	

	while (true) {
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
		int ret = select(_sock + 1, &fdRead, &fdWrite, &fdExp, NULL);
		if (ret < 0) {
			//����
			printf("select ������� \n");
			break;
		}


		
		if (FD_ISSET(_sock, &fdRead))  {
			FD_CLR(_sock, &fdRead);
			//4.accept �ȴ����ܿͻ�������
			sockaddr_in clientAddr = {};
			int nAddrLen = sizeof(clientAddr);
			SOCKET _cSocket = INVALID_SOCKET;
			_cSocket = accept(_sock, (sockaddr*)&clientAddr, &nAddrLen);
			if (INVALID_SOCKET == _cSocket) {
				printf("���󣬽��ܵ���Ч�Ŀͻ���socket.. \n");
				return -1;
			}
			printf("�¿ͻ��˼��룺Socket = %d, IP = %s \n", (int)_cSocket, inet_ntoa(clientAddr.sin_addr));
			g_clients.push_back(_cSocket);
		}

		for (size_t n = 0; n < fdRead.fd_count; n++) {
			if (-1 == processor(fdRead.fd_array[n])) {
				auto iter = find(g_clients.begin(), g_clients.end(), fdRead.fd_array[n]);
				if (iter != g_clients.end()) {
					g_clients.erase(iter);
				}
			}
		}
	}


	for (int n = g_clients.size() - 1; n >= 0; n--) {
		closesocket(g_clients[n]);
	}

	//8 �ر��׽���closesocket
	closesocket(_sock);


	WSACleanup();
	return 0;
}