#define WIN32_LEAN_AND_MEAN
#include <iostream>
#include <windows.h>
#include <WinSock2.h>
#include <stdio.h>

int  main() {

	//windows 启动socket环境
	WORD ver = MAKEWORD(2, 2);
	WSADATA data;
	WSAStartup(ver, &data);

	//用Socket api建立简易TCP客户端
	//1.建立一个socket
	SOCKET _sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == _sock) {
		printf("错误，建立socket失败");
		return -1;
	}
	else {
		printf("建立socket成功..");
	}

	//2.连接服务器 connect
	sockaddr_in _sin = {};
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(4567);
	_sin.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	int ret =  connect(_sock, (sockaddr *) & _sin, sizeof(sockaddr_in));
	if (ret == SOCKET_ERROR) {
		printf("错误，连接socket失败");
	}
	else {
		printf("建立socket 成功..");
	}
	//3.接收服务器信息 recv
	char recvBuf[256] = {};
	int nLen = recv(_sock, recvBuf, 256, 0);
	if (nLen > 0) {
		printf("接收到数据：%s   \n", recvBuf);
	}
	//4.关闭套接字closesocket
	closesocket(_sock);
	WSACleanup();
	getchar();
	return 0;
}