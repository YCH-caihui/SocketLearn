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
	int ret =  connect(_sock, (sockaddr *) & _sin, sizeof(sockaddr_in));
	if (ret == SOCKET_ERROR) {
		printf("��������socketʧ�� \n");
		return -1;
	}
	else {
		printf("����socket �ɹ�.. \n");
	
	}

	char cmdBuf[128] = {};
	while (true) {
		//3.������������
		printf("����������:");
		scanf("%s", cmdBuf);
		//4.������������
		if (0 == strcmp(cmdBuf,"exit")) {
			printf("�յ��������");
			break;
		}
		else if( 0 == strcmp(cmdBuf, "login")) {
			Login login;
			strcpy(login.userName, "yyds");
			strcpy(login.passWord, "yyds passoword");
			send(_sock, (const char*)&login, sizeof(Login), 0);

			//���շ��������ص����� 
			LoginResult loginResult = {};
			recv(_sock, (char*)&loginResult, sizeof(LoginResult), 0);
			printf("LoginResult: %d    \n", loginResult.result);
		}
		else if (0 == strcmp(cmdBuf, "logout")) {
			LogOut  logOut = {};
			strcpy(logOut.userName, "yyds");
			send(_sock, (const char *)&logOut, sizeof(LogOut), 0);

			//���ܷ��������ص�����
			LogOutResult logOutResult = {};
			recv(_sock, (char *)&logOutResult, sizeof(LogOutResult), 0);
			printf(" LogOutResult : %d   \n", logOutResult.result);
		} 
		else {
			printf("��֧�ֵ��������������  \n");
		}

	}

	printf("�������..\n");
	
	//7.�ر��׽���closesocket
	closesocket(_sock);
	WSACleanup();
	getchar();
	return 0;
}