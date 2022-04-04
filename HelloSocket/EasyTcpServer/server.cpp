#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include <windows.h>
#include <WinSock2.h>
#include <stdio.h>


enum CMD {
	CMD_LOGIN, 
	CMD_LOGOUT,
	CMD_ERROR
};

//��Ϣͷ
struct DataHeader {
	short dataLenth;
	short cmd;
};

//��Ϣ��
struct Login {
	char userName[32];
	char passWord[32];
};


struct LoginResult {
	int result;
};

struct LogOut {
	char userName[32];
};

struct LogOutResult {
	int result;
};

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

	char _recvBuf[128] = {};
	while (true) {

		DataHeader header = {};
		int nLen = recv(_cSocket, (char *)&header, sizeof(DataHeader), 0);
		if (nLen <= 0)
		{
			printf("�ͻ������˳����������");
			break;
		}
		printf("�յ�����: %d ���ݳ���:%d \n", header.cmd, header.dataLenth);

		switch (header.cmd) {
		case CMD_LOGIN: {
			Login login = {};
			recv(_cSocket, (char*)&login, sizeof(Login), 0);
			//�����ж��û��������Ƿ���ȷ�Ĺ���

			LoginResult resutIn = { 1 };
			send(_cSocket, (char*)&header, sizeof(DataHeader), 0);
			send(_cSocket, (char*)&resutIn, sizeof(LoginResult), 0);
			break;
		}
		case CMD_LOGOUT: {
			LogOut loginOut = {};
			recv(_cSocket, (char*)&loginOut, sizeof(LogOut), 0);

			LogOutResult resultOut = { 1 };
			send(_cSocket, (char*)&header, sizeof(DataHeader), 0);
			send(_cSocket, (char*)&resultOut, sizeof(LogOutResult), 0);
			break;
		}
		default: {
			header.cmd = CMD_ERROR;
			header.dataLenth = 0;
			send(_cSocket, (char*)&header, sizeof(DataHeader), 0);
			break;
		}
		} 
	
	}
	//8 �ر��׽���closesocket
	closesocket(_sock);


	WSACleanup();
	return 0;
}