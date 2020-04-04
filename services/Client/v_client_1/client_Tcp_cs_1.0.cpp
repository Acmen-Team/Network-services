#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <stdio.h>
#include<iostream>

#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")

/*
	用socket api建立一个简易TCP客户端（c-s模型）
	步骤：
		1.建立一个socket
		2.连接服务器 connect
		3.接收服务器信息 recv
		4.关闭套接字 closesocket
*/

int main_block(void)	//因各版本在同一vs项目下，故为了避免生成时main函数冲突加block
{
	//启动Windows socket 2.x环境
	WORD ver = MAKEWORD(2, 2);
	WSADATA data;

	//判断网络库打开是否成功
	if (0 != WSAStartup(ver, &data))
	{
		//获取错误码
		int eroCode = WSAGetLastError();
		printf("ERROR(错误码: %d)：网络库打开失败！\n", eroCode);
		//清理网络库
		WSACleanup();
		return 0;
	}
	//1.建立一个socket
	SOCKET _cliSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	//判断SOCKET是否创建成功
	if (INVALID_SOCKET == _cliSock)
	{
		//获取错误码
		int eroCode = WSAGetLastError();
		printf("ERROR(错误码: %d)：SOCKET创建失败！\n", eroCode);
		//清理网络库
		WSACleanup();
		return 0;
	}
	//2.连接服务器 connect
	/*
		int connect(
			SOCKET s,
			const sockaddr *name,
			int namelen
		);连接服务器			详细信息:https://docs.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-connect
		返回值：
			0 代表绑定成功
			否则将返回SOCKET_ERROR的值，并且可以通过调用WSAGetLastError来检索特定的错误代码。
	*/
	struct sockaddr_in _cli;
	_cli.sin_family = AF_INET;
	_cli.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	_cli.sin_port = htons(23323);
	//判断是否连接成功
	if (SOCKET_ERROR == connect(_cliSock, (const sockaddr*)&_cli, sizeof(_cli)))
	{
		//获取错误码
		int eroCode = WSAGetLastError();
		printf("ERROR(错误码: %d)：数据发送失败！\n", eroCode);
		//清除套接字
		closesocket(_cliSock);
		//清理网络库
		WSACleanup();
		return 0;
	}
	//3.接收服务器信息 recv
	/*
		int recv(
			SOCKET s,
			char *buf,
			int len,
			int flags
		);接收服务器信息		详细信息:https://docs.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-recv
		返回值：
			接收成功则返回接收的字节数
			否则返回SOCKET_ERROR的值，通过调用WSAGetLastError来检索特定的错误代码。
	*/
	//判断是否接收成功
	char buf[1024] = { 0 };
	if (SOCKET_ERROR == recv(_cliSock, buf, 1023, 0))
	{
		//获取错误码
		int eroCode = WSAGetLastError();
		printf("ERROR(错误码: %d)：数据接收失败！\n", eroCode);
		//清除套接字
		closesocket(_cliSock);
		//清理网络库
		WSACleanup();
		return 0;
	}
	else
	{
		printf("服务器消息:%s\n", buf);
	}
	//6.关闭套接字 closesocket
	closesocket(_cliSock);
	//清理网络库
	WSACleanup();
	return std::cin.get();
}