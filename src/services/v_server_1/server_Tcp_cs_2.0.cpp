#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <iostream>
//#include <Windows.h>

#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")

/*
	简易TCP服务端业务逻辑更新，处理持续性请求，详情见流程图
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
		//获取错误码
		int eroCode = WSAGetLastError();
		printf("ERROR(错误码: %d)：网络库打开失败！\n", eroCode);
		//清理网络库
		WSACleanup();
		return 0;
	}
	//创建socket
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
	_ser.sin_port = htons(23324);
	//绑定端口
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

	struct sockaddr_in _cli;
	int _clisize = sizeof(_cli);
	//等待客户端连接
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
	//定义收发缓冲区
	char buf_R[1024] = { 0 };
	char buf_S[1024] = { 0 };
	while (true)
	{
		//接收客户端请求
		int bufLen = recv(_cliSock, buf_R, 1023, 0);
		if (SOCKET_ERROR == bufLen)
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
		else if (bufLen <= 0)
		{
			printf("客户端以退出，任务结束！");
			break;
		}
		else
		{
			if (strcmp(buf_R, "getTime") == 0)
			{
				SYSTEMTIME sys;
				GetLocalTime(&sys);//windows系统API，获取当前系统时间
				sprintf_s(buf_S, "%4d/%02d/%02d %02d:%02d:%02d 星期%1d\n", sys.wYear, sys.wMonth, sys.wDay, sys.wHour, sys.wMinute, sys.wSecond,sys.wDayOfWeek);
			}
			else if (strcmp(buf_R, "getName") == 0)
			{
				strcpy(buf_S, "ServerName:server_Tcp_CS");
			}
			else
			{
				strcpy(buf_S, "请求出错，请重新输入请求！");
			}
			//发送请求的数据
			if (SOCKET_ERROR == send(_cliSock, buf_S, strlen(buf_S), 0))
			{
				//获取错误码
				int eroCode = WSAGetLastError();
				printf("ERROR(错误码: %d)：数据发送失败！\n", eroCode);
				//清除套接字
				closesocket(_cliSock);
				break;
			}
		}
	}

	//6.关闭套接字 closesocket
	closesocket(_serSock);
	//清理网络库
	WSACleanup();
	return 0;
}