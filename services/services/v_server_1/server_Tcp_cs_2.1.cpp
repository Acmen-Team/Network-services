#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")
using namespace std;

/*
	简易TCP服务端业务逻辑更新，处理持续性请求，详情见流程图
	消息类型:结构化数据
*/

//定义结构化数据
struct dataPackage
{
	int _mVer;
	int _dVer;
	char serName[20];
	char serUser[20];
};

int main()
{
	//打开网络库
	WORD ver = MAKEWORD(2, 2);
	WSADATA datd;
	if (0 != WSAStartup(ver, &datd))
	{
		int eroCode = WSAGetLastError();
		printf("ERROR(错误码: %d)：网络库打开失败！\n", eroCode);
		WSACleanup();
		return 0;
	}
	//创建SOCKET
	SOCKET _serSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == _serSock)
	{
		int eroCode = WSAGetLastError();
		printf("ERROR(错误码: %d)：SOCKET创建失败！\n", eroCode);
		WSACleanup();
		return 0;
	}
	//绑定端口
	struct sockaddr_in _ser;
	_ser.sin_family = AF_INET;
	_ser.sin_addr.S_un.S_addr = INADDR_ANY;
	_ser.sin_port = htons(33233);
	if (SOCKET_ERROR == bind(_serSock, (const struct sockaddr*)&_ser, sizeof(_ser)))
	{
		int eroCode = WSAGetLastError();
		printf("ERROR(错误码: %d)：SOCKET绑定失败！\n", eroCode);
		closesocket(_serSock);
		WSACleanup();
		return 0;
	}
	//监听端口
	if (SOCKET_ERROR == listen(_serSock, 5))
	{
		int eroCode = WSAGetLastError();
		printf("ERROR(错误码: %d)：监听失败！\n", eroCode);
		closesocket(_serSock);
		WSACleanup();
		return 0;
	}
	//等待客户端连接
	struct sockaddr_in _cli;
	int _clisize = sizeof(_cli);
	SOCKET _cliSock = accept(_serSock, (struct sockaddr*)&_cli, &_clisize);
	if (INVALID_SOCKET == _cliSock)
	{
		//获取错误码
		int eroCode = WSAGetLastError();
		printf("ERROR(错误码: %d)：客户端连接失败！\n", eroCode);
		//清除套接字
		closesocket(_serSock);
		//清理网络库
		WSACleanup();
		return 0;
	}
	else
	{
		printf("新客户端已连接！ IP = %s\n", inet_ntoa(_cli.sin_addr));
	}
	//处理机制
	char buf_R[1024] = { 0 };
	dataPackage dp = { 2, 1, "server_Tcp_cs", "Holy-YxY" };
	while (true)
	{
		int bufLen = recv(_cliSock, buf_R, 1023, 0);
		if (SOCKET_ERROR == bufLen)
		{
			//获取错误码
			int eroCode = WSAGetLastError();
			printf("ERROR(错误码: %d)：数据接收失败！\n", eroCode);
			//清除套接字
			closesocket(_cliSock);
			break;
		}
		else if (bufLen <= 0)
		{
			printf("客户端以退出，任务结束！\n");
			//清除套接字
			closesocket(_cliSock);
			break;
		}
		else
		{
			if (strcmp(buf_R, "getServer") == 0)
			{
				if (SOCKET_ERROR == send(_cliSock, (const char*)&dp, sizeof(dataPackage), 0))
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
			}
		}
	}

	//清除套接字
	closesocket(_serSock);
	//清理网络库
	WSACleanup();
	system("pause");
	return 0;
}