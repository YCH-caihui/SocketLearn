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
		else if( 0 == strcmp(cmdBuf, "login")) {
			Login login;
			strcpy(login.userName, "yyds");
			strcpy(login.passWord, "yyds passoword");
			send(_sock, (const char*)&login, sizeof(Login), 0);

			//接收服务器返回的数据 
			LoginResult loginResult = {};
			recv(_sock, (char*)&loginResult, sizeof(LoginResult), 0);
			printf("LoginResult: %d    \n", loginResult.result);
		}
		else if (0 == strcmp(cmdBuf, "logout")) {
			LogOut  logOut = {};
			strcpy(logOut.userName, "yyds");
			send(_sock, (const char *)&logOut, sizeof(LogOut), 0);

			//接受服务器返回的数据
			LogOutResult logOutResult = {};
			recv(_sock, (char *)&logOutResult, sizeof(LogOutResult), 0);
			printf(" LogOutResult : %d   \n", logOutResult.result);
		} 
		else {
			printf("不支持的命令，请重新输入  \n");
		}

	}

	printf("任务结束..\n");
	
	//7.关闭套接字closesocket
	closesocket(_sock);
	WSACleanup();
	getchar();
	return 0;
}