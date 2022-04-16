#define WIN32_LEAN_AND_MEAN
#include <iostream>
#include <windows.h>
#include <WinSock2.h>
#include <stdio.h>
#include <thread>
#include "EasyTcpClient.h"

/*
enum CMD {
	CMD_LOGIN,
	CMD_LOGIN_RESULT,
	CMD_LOGOUT,
	CMD_LOGOUT_RESULT,
	CMD_NEW_USER_JOIN,
	CMD_ERROR
};

//消息头
struct DataHeader {
	short dataLenth;
	short cmd;
};

//消息体
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
		printf("与服务器断开链接,任务结束");
		return -1;
	}

	DataHeader* header = (DataHeader*)szRecv;


	switch (header->cmd) {
	case CMD_LOGIN_RESULT: {
		recv(_cSocket, szRecv + sizeof(DataHeader), sizeof(LoginResult) - sizeof(DataHeader), 0);
		LoginResult* login = (LoginResult*)&szRecv;
		printf("接受到CMD_LOGIN_RESULT  数据长度:%d  Result:%d   \n", login->dataLenth, login->result);
	
		break;
	}
	case CMD_LOGOUT_RESULT: {
		recv(_cSocket, szRecv + sizeof(DataHeader), sizeof(LogOutResult) - sizeof(DataHeader), 0);
		LogOutResult* login = (LogOutResult*)&szRecv;
		printf("接受到CMD_LOGOUT_RESULT  数据长度:%d  Result:%d   \n", login->dataLenth, login->result);
		break;
	}
	case CMD_NEW_USER_JOIN: {
		recv(_cSocket, szRecv + sizeof(DataHeader), sizeof(NewUserJoin) - sizeof(DataHeader), 0);
		NewUserJoin* login = (NewUserJoin*)&szRecv;
		printf("接受到CMD_NEW_USER_JOIN  数据长度:%d  sock:%d   \n", login->dataLenth, login->sock);
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

bool g_Run = true;

void cmdThread(SOCKET sock) {
	char cmdBuf[256] = {};
	while (true) {
	scanf("%s", cmdBuf);
	if (0 == strcmp(cmdBuf, "exit")) {
		g_Run = false;
		printf("退出 \n");
		break;
	}
	else if (0 == strcmp(cmdBuf, "login")) {
		Login login;
		strcpy(login.userName, "yyds");
		strcpy(login.passWord, "yyds password");
		send(sock, (const char*)&login, sizeof(Login), 0);
	}
	else if (0 == strcmp(cmdBuf, "logout")) {
		LogOut logout;
		strcpy(logout.userName, "lyd");
		send(sock, (const char*)&logout, sizeof(LogOut), 0);
	}
}
 }
 */

bool g_bRun = true;
void cmdThread(EasyTcpClient * client) {
	char cmdBuf[256] = {};
	while (true) {
		scanf("%s", cmdBuf);
		if (0 == strcmp(cmdBuf, "exit")) {
			client->close();
			printf("退出 \n");
			break;
		}
		else if (0 == strcmp(cmdBuf, "login")) {
			Login login;
			strcpy(login.userName, "yyds");
			strcpy(login.passWord, "yyds password");
			client->sendData(&login);
		}
		else if (0 == strcmp(cmdBuf, "logout")) {
			LogOut logout;
			strcpy(logout.userName, "lyd");
			client->sendData(&logout);
		}
	}
}
int  main() {

	/*
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
	int ret =  connect(_sock, (sockaddr *)&_sin, sizeof(sockaddr_in));
	if (ret == SOCKET_ERROR) {
		printf("错误，连接socket失败 \n");
		return -1;
	}
	else {
		printf("建立socket 成功.. \n");
	}

	//启动线程
	std::thread t1(cmdThread, _sock);
	t1.detach();
	while (g_Run) {

		fd_set fdReads;
		FD_ZERO(&fdReads);
		FD_SET(_sock, &fdReads);
		timeval t = { 0, 0 };
		int ret = select(_sock, &fdReads, 0, 0, &t);
		if (ret < 0) {
			printf("select任务结束  \n");
			break;
		}

		if (FD_ISSET(_sock, &fdReads)) {
			FD_CLR(_sock, &fdReads);
			if (-1 == processor(_sock)) {
				printf("select任务结束\n");
				break;
			}

		}

	//	printf("空闲时间处理其他业务.. \n");
		
		Sleep(3);

	}

	printf("任务结束..\n");
	
	//7.关闭套接字closesocket
	closesocket(_sock);
	WSACleanup();
	*/

	EasyTcpClient client;
	client.initSocket();
	client.connectSocket("127.0.0.1", 4567);
	std::thread t1(cmdThread, &client);
	t1.detach();


	while (client.isRun()) {
		client.onRun();
	}

	client.close();
	
	getchar();
	return 0;
}