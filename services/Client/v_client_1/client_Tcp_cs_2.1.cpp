#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")

//定义结构化数据
struct  dataPackage
{
	int _mVer;
	int _dVer;
	char serName[20];
	char serUser[20];
};

int main(void)
{
	//打开网络库
	WORD ver = MAKEWORD(2, 2);
	WSADATA data;
	if (0 != WSAStartup(ver, &data))
	{
		int errCode = WSAGetLastError();
		printf("ERROR(错误码：%d):网络库打开失败", errCode);
		return 0;
	}
	//创建SOCKET
	SOCKET _cliSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == _cliSock)
	{
		int errCode = WSAGetLastError();
		printf("ERROR(错误码：%d):SOCKET创建失败", errCode);
		//清理网络库
		WSACleanup();
		return 0;
	}
	//连接服务器
	struct sockaddr_in _cli;
	_cli.sin_family = AF_INET;
	_cli.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	_cli.sin_port = htons(33233);
	if (SOCKET_ERROR == connect(_cliSock, (const struct sockaddr*)&_cli, sizeof(_cli)))
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
	printf("getServer  获取服务器信息\n");
	printf("quit	   客户端退出\n");
	while (true)
	{
		char buf_R[1024] = { 0 };
		printf("调用请求:");
		scanf("%s", buf_S);
		if (strcmp(buf_S, "quit") == 0)
		{
			break;
		}
		if (SOCKET_ERROR == send(_cliSock, buf_S, strlen(buf_S), 0))
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
		dataPackage *dp = (dataPackage*)buf_R;
		printf("serName:%s serUser:%s mVer:%d dVer:%d\n", dp->serName, dp->serUser, dp->_mVer, dp->_mVer);
	}

	//清除套接字
	closesocket(_cliSock);
	//清理网络库
	WSACleanup();
	system("pause");
	return 0;
}


