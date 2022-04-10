#define WIN32_LEAN_AND_MEAN
#include <iostream>
#include <windows.h>
#include <WinSock2.h>
#include <stdio.h>


enum CMD {
	CMD_LOGIN,
	CMD_LOGIN_RESULT,
	CMD_LOGOUT,
	CMD_LOGOUT_RESULT,
	CMD_NEW_USER_JOIN,
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

struct NewUserJoin : public DataHeader {

	int sock;

	NewUserJoin() {
		dataLenth = sizeof(NewUserJoin);
		cmd = CMD_NEW_USER_JOIN;
		sock = 0;
	}
};

int processor(SOCKET _cSocket) {
	char szRecv[1024] = {};

	int nLen = recv(_cSocket, szRecv, sizeof(DataHeader), 0);
	if (nLen <= 0)
	{
		printf("��������Ͽ�����,�������");
		return -1;
	}

	DataHeader* header = (DataHeader*)szRecv;


	switch (header->cmd) {
	case CMD_LOGIN_RESULT: {
		recv(_cSocket, szRecv + sizeof(DataHeader), sizeof(LoginResult) - sizeof(DataHeader), 0);
		LoginResult* login = (LoginResult*)&szRecv;
		printf("���ܵ�CMD_LOGIN_RESULT  ���ݳ���:%d  Result:%d   \n", login->dataLenth, login->result);
	
		break;
	}
	case CMD_LOGOUT_RESULT: {
		recv(_cSocket, szRecv + sizeof(DataHeader), sizeof(LogOutResult) - sizeof(DataHeader), 0);
		LogOutResult* login = (LogOutResult*)&szRecv;
		printf("���ܵ�CMD_LOGOUT_RESULT  ���ݳ���:%d  Result:%d   \n", login->dataLenth, login->result);
		break;
	}
	case CMD_NEW_USER_JOIN: {
		recv(_cSocket, szRecv + sizeof(DataHeader), sizeof(NewUserJoin) - sizeof(DataHeader), 0);
		NewUserJoin* login = (NewUserJoin*)&szRecv;
		printf("���ܵ�CMD_NEW_USER_JOIN  ���ݳ���:%d  sock:%d   \n", login->dataLenth, login->sock);
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

	//��Socket api��������TCP�ͻ���
	//1.����һ��socket
	SOCKET _sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == _sock) {
		printf("���󣬽���socketʧ��");
		return -1;
	}
	else {
		printf("����socket�ɹ�..");
	}

	//2.���ӷ����� connect
	sockaddr_in _sin = {};
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(4567);
	_sin.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	int ret =  connect(_sock, (sockaddr *)&_sin, sizeof(sockaddr_in));
	if (ret == SOCKET_ERROR) {
		printf("��������socketʧ�� \n");
		return -1;
	}
	else {
		printf("����socket �ɹ�.. \n");
	}

	char cmdBuf[128] = {};
	while (true) {

		fd_set fdReads;
		FD_ZERO(&fdReads);
		FD_SET(_sock, &fdReads);
		timeval t = { 0, 0 };
		int ret = select(_sock, &fdReads, 0, 0, &t);
		if (ret < 0) {
			printf("select�������  \n");
			break;
		}

		if (FD_ISSET(_sock, &fdReads)) {
			FD_CLR(_sock, &fdReads);
			if (-1 == processor(_sock)) {
				printf("select�������\n");
				break;
			}

		}

		printf("����ʱ�䴦������ҵ��.. \n");
		Login login;
		strcpy(login.userName, "yyds");
		strcpy(login.passWord, "yyds password");
		send(_sock, (const char *)&login, sizeof(Login), 0);
		Sleep(3);

	}

	printf("�������..\n");
	
	//7.�ر��׽���closesocket
	closesocket(_sock);
	WSACleanup();
	getchar();
	return 0;
}