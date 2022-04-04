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

//消息头
struct DataHeader {
	short dataLenth;
	short cmd;
};

//消息体
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
			Login login = {"yyds", "yyds_password"};
			DataHeader dh = {sizeof(login), CMD_LOGIN};

			send(_sock, (const char *)&dh, sizeof(DataHeader), 0);
			send(_sock, (const char*)&login, sizeof(Login), 0);

			//接收服务器返回的数据 
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

			//接受服务器返回的数据
			DataHeader retHeader = {};
			LogOutResult logOutResult = {};
			recv(_sock, (char*)&retHeader, sizeof(DataHeader), 0);
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