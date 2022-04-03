#define WIN32_LEAN_AND_MEAN
#include <iostream>
#include <windows.h>
#include <WinSock2.h>


int  main() {

	//windows Æô¶¯socket»·¾³
	WORD ver = MAKEWORD(2, 2);
	WSADATA data;
	WSAStartup(ver, &data);


	WSACleanup();
	return 0;
}