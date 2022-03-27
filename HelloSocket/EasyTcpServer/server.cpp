#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include <windows.h>
#include <WinSock2.h>
#include <stdio.h>


int  main() {

	//windows 启动socket环境
	WORD ver = MAKEWORD(2, 2);
	WSADATA data;
	WSAStartup(ver, &data);

	//用socket API建立简易TCP 服务器
	//1.建立一个socket 套接字
 	SOCKET _sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	//2.bind 绑定用于接受客户端连接的网络端口
	sockaddr_in _sin = {};
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(4567);
	_sin.sin_addr.S_un.S_addr = INADDR_ANY; //inet_addr("127.0.0.1");
	if (bind(_sock, (sockaddr*)&_sin, sizeof(_sin)) == SOCKET_ERROR) {
		printf("error,绑定用于接受客户端连接的网络端口失败\n");
		return -1;
	}
	else {
		printf("绑定网络端口成功。\n");
	}
	//3.listen监听网络端口
	if (listen(_sock, 5) == SOCKET_ERROR) {
		printf("错误，监听网络端口失败  \n");
		return -2;
	}
	else {
		printf("监听网络端口成功   \n");
	}
	//4.accept 等待接受客户端连接
	sockaddr_in clientAddr = {};
	int nAddrLen = sizeof(clientAddr);
	SOCKET _cSocket = INVALID_SOCKET;
	while (true) {
		_cSocket = accept(_sock, (sockaddr*)&clientAddr, &nAddrLen);
		if (INVALID_SOCKET == _cSocket) {
			printf("错误，接受到无效的客户端socket.. \n");
			return -1;
		}
		printf("新客户端加入：IP = %s \n", inet_ntoa(clientAddr.sin_addr));
		//5. send想客户端发送一条数据 
		char msgBuf[] = "Hello, I'm Server..";
		send(_cSocket, msgBuf, strlen(msgBuf) + 1, 0);
	}
	//6 关闭套接字closesocket
	closesocket(_sock);


	WSACleanup();
	return 0;
}