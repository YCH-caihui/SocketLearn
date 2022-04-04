#define WIN32_LEAN_AND_MEAN
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

struct DataPackage {
	int age;
	char name[32];
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
			Login login = {"yyds", "yyds_password"};
			DataHeader dh = {sizeof(login), CMD_LOGIN};

			send(_sock, (const char *)&dh, sizeof(DataHeader), 0);
			send(_sock, (const char*)&login, sizeof(Login), 0);

			//���շ��������ص����� 
			DataHeader retHeader = {};
			LoginResult loginResult = {};
			recv(_sock, (char *)&retHeader, sizeof(DataHeader), 0);
			recv(_sock, (char*)&loginResult, sizeof(LoginResult), 0);
			printf("LoginResult: %d    \n", loginResult.result);
		}
		else if (0 == strcmp(cmdBuf, "logout")) {
			LogOut  logOut = {};
			DataHeader dh = {sizeof(logOut), CMD_LOGOUT };

			send(_sock, (const char*)&dh, sizeof(DataHeader), 0);
			send(_sock, (const char *)&logOut, sizeof(LogOut), 0);

			//���ܷ��������ص�����
			DataHeader retHeader = {};
			LogOutResult logOutResult = {};
			recv(_sock, (char*)&retHeader, sizeof(DataHeader), 0);
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