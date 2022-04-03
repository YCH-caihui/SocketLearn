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
		printf("错误，连接socket失败 \n");
		return -1;
	}
	else {
		printf("建立socket 成功.. \n");
	
	}

	char cmdBuf[128] = {};
	while (true) {
		//3.输入请求命令
		printf("请输入命令:");
		scanf("%s", cmdBuf);
		//4.处理请求命令
		if (0 == strcmp(cmdBuf,"exit")) {
			printf("收到任务结束");
			break;
		}
		else {
			//5.向服务器发送请求命令
			send(_sock, cmdBuf, strlen(cmdBuf) + 1, 0);
		}

		//6.接收服务器信息 recv
		char recvBuf[256] = {};
		int nLen = recv(_sock, recvBuf, 256, 0);
		if (nLen > 0) {
			DataPackage* dp = (DataPackage*)recvBuf;
			printf("接收到数据：年龄%d   姓名:%s  \n", dp->age, dp->name);
		}

	}

	printf("任务结束..\n");
	
	//7.关闭套接字closesocket
	closesocket(_sock);
	WSACleanup();
	getchar();
	return 0;
}