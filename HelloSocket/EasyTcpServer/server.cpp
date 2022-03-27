#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include <windows.h>
#include <WinSock2.h>
#include <stdio.h>


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
	while (true) {
		_cSocket = accept(_sock, (sockaddr*)&clientAddr, &nAddrLen);
		if (INVALID_SOCKET == _cSocket) {
			printf("���󣬽��ܵ���Ч�Ŀͻ���socket.. \n");
			return -1;
		}
		printf("�¿ͻ��˼��룺IP = %s \n", inet_ntoa(clientAddr.sin_addr));
		//5. send��ͻ��˷���һ������ 
		char msgBuf[] = "Hello, I'm Server..";
		send(_cSocket, msgBuf, strlen(msgBuf) + 1, 0);
	}
	//6 �ر��׽���closesocket
	closesocket(_sock);


	WSACleanup();
	return 0;
}