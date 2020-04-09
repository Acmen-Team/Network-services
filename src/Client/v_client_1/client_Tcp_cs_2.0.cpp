#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")

/*
	简易TCP客户端业务逻辑更新，发送持续性请求，详情见流程图
	消息类型:纯字符串网络消息
		优点：处理简单，方便快捷
		缺点：传递大量数据时，字符串解析消耗大
*/

int main_blocks(void)
{
	WORD ver = MAKEWORD(2, 2);
	WSADATA data;
	//打开网络库
	if (0 != WSAStartup(ver, &data))
	{
		int eroCode = WSAGetLastError();
		printf("ERROR(错误码: %d)：网络库打开失败！\n", eroCode);
		WSACleanup();
		return 0;
	}
	//创建socket
	SOCKET _cliSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == _cliSock)
	{
		int eroCode = WSAGetLastError();
		printf("ERROR(错误码: %d)：SOCKET创建失败！\n", eroCode);
		WSACleanup();
		return 0;
	}
	//连接服务器
	struct sockaddr_in _cli;
	_cli.sin_family = AF_INET;
	_cli.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	_cli.sin_port = htons(23324);
	if (SOCKET_ERROR == connect(_cliSock, (const sockaddr*)&_cli, sizeof(_cli)))
	{
		//获取错误码
		int eroCode = WSAGetLastError();
		printf("ERROR(错误码: %d)：服务器连接失败！\n", eroCode);
		//清除套接字
		closesocket(_cliSock);
		//清理网络库
		WSACleanup();
		return 0;
	}
	//发送机制
	char buf_S[1024];
	printf("------当前可用指令------\n");
	printf("getName  获取服务器名\n");
	printf("getTime  获取服务器时间\n");
	while (true)
	{
		char buf_R[1024] = { 0 };
		printf("调用请求:");
		scanf("%s", buf_S);
		if (SOCKET_ERROR == send(_cliSock, buf_S, strlen(buf_S), 0))
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
		if (SOCKET_ERROR == recv(_cliSock, buf_R, 1023, 0))
		{
			//获取错误码
			int eroCode = WSAGetLastError();
			printf("ERROR(错误码: %d)：数据接收失败！\n", eroCode);
			//清除套接字
			closesocket(_cliSock);
			//清理网络库
			WSACleanup();
			break;
		}
		printf("%s\n", buf_R);
	}

	//清除套接字
	closesocket(_cliSock);
	//清理网络库
	WSACleanup();
	return 0;
}