#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS

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
		//5.���ܿͻ�������
		int nLen = recv(_cSocket, _recvBuf, 128, 0);
		if (nLen <= 0) {
			printf("�ͻ������Ƴ����������");
			break;
		}

		printf("�յ�����: %s", _recvBuf);

		//6.��������
		if (0 == strcmp(_recvBuf, "getName")) {
			//7. send��ͻ��˷���һ������ 
			char msgBuf[] = "xiao qiang..";
			send(_cSocket, msgBuf, strlen(msgBuf) + 1, 0);
		}
		else if (0 == strcmp(_recvBuf, "getAge")) {
			//7. send��ͻ��˷���һ������ 
			char msgBuf[] = "80.";
			send(_cSocket, msgBuf, strlen(msgBuf) + 1, 0);
		}
		else if (0 == strcmp(_recvBuf, "getInfo")) {
			DataPackage dp = {80, "Сǿ"};
			send(_cSocket, (const char*)&dp, sizeof(DataPackage), 0);
		}
		else {
			//7. send��ͻ��˷���һ������ 
			char msgBuf[] = "???????..";
			send(_cSocket, msgBuf, strlen(msgBuf) + 1, 0);
		}
	
	
	
	}
	//8 �ر��׽���closesocket
	closesocket(_sock);


	WSACleanup();
	return 0;
}