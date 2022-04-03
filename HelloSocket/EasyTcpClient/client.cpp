#define WIN32_LEAN_AND_MEAN
#include <iostream>
#include <windows.h>
#include <WinSock2.h>
#include <stdio.h>


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
		else {
			//5.�������������������
			send(_sock, cmdBuf, strlen(cmdBuf) + 1, 0);
		}

		//6.���շ�������Ϣ recv
		char recvBuf[256] = {};
		int nLen = recv(_sock, recvBuf, 256, 0);
		if (nLen > 0) {
			DataPackage* dp = (DataPackage*)recvBuf;
			printf("���յ����ݣ�����%d   ����:%s  \n", dp->age, dp->name);
		}

	}

	printf("�������..\n");
	
	//7.�ر��׽���closesocket
	closesocket(_sock);
	WSACleanup();
	getchar();
	return 0;
}