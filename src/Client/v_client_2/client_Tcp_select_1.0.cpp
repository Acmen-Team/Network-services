#ifdef _WIN32
	#define _WINSOCK_DEPRECATED_NO_WARNINGS
	#define _CRT_SECURE_NO_WARNINGS

	#include <WinSock2.h>
	#pragma comment(lib, "ws2_32.lib")
#else
	#include <unistd.h>
	#include <arpa/inet.h>
	#include <string.h>
	#include <vector>

	#define SOCKET int
	#define INVALID_SOCKET  (SOCKET)(~0)
	#define SOCKET_ERROR            (-1)
#endif // _WIN32

#include <stdio.h>
#include <thread>
//发送消息线程函数
void _message(SOCKET);
bool block = true;

/*
	select模型
	实现多人群聊功能
*/

int main_blocks_2_0(void)
{
#ifdef _WIN32
	WORD ver = MAKEWORD(2, 2);
	WSADATA data;
	//打开网络库
	if (0 != WSAStartup(ver, &data))
	{
		printf("ERROR：网络库打开失败！\n");
		//清理网络库
		WSACleanup();
		return 0;
	}
#endif
	//创建socket
	SOCKET _cliSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == _cliSock)
	{
		printf("ERROR：SOCKET创建失败！\n");
	#ifdef _WIN32
		//清除套接字
		closesocket(_cliSock);
		//清理网络库
		WSACleanup();
	#else
		close(_cliSock);
	#endif
		return 0;
	}
	//连接服务器
	struct sockaddr_in _cli;
	_cli.sin_family = AF_INET;
#ifdef _WIN32
	_cli.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
#else
	_cli.sin_addr.s_addr = inet_addr("127.0.0.1");
#endif
	_cli.sin_port = htons(8989);
	if (SOCKET_ERROR == connect(_cliSock, (const sockaddr*)&_cli, sizeof(_cli)))
	{
		printf("ERROR：服务器连接失败，请联系服务商！\n");
	#ifdef _WIN32
		//清除套接字
		closesocket(_cliSock);
		//清理网络库
		WSACleanup();
	#else
		close(_cliSock);
	#endif
		return 0;
	}


	fd_set sockArry;
	FD_ZERO(&sockArry);
	FD_SET(_cliSock, &sockArry);

	struct timeval timeout;
	timeout.tv_sec = 2;
	timeout.tv_usec = 0;

	std::thread send_M(_message, _cliSock);

	while (block)
	{
		fd_set ac_sock = sockArry;
		int s_result = select(_cliSock + 1, &ac_sock, NULL, NULL, &timeout);
		if (s_result == 0)
		{
			continue;
		}
		else if (s_result > 0)
		{
			char buf_R[1024] = { 0 };
			if (SOCKET_ERROR == recv(_cliSock, buf_R, 1023, 0))
			{
				printf("ERROR：数据接收失败，请重新登陆。\n");
				break;
			}
			printf("\t\t\t%s\n", buf_R);
		}
	}
	send_M.join();
#ifdef _WIN32
	//清除套接字
	closesocket(_cliSock);
	//清理网络库
	WSACleanup();
#else
	close(_cliSock);
#endif
	return 0;
}
//发送消息线程
void _message(SOCKET sock)
{
	while (true)
	{
		char buf_S[1024] = { 0 };
		scanf("%s", buf_S);
		if (strcmp(buf_S, "exit") == 0)
		{
			printf("客户端已关闭，任务结束!\n");
			block = false;
			break;
		}
		else
		{
			send(sock, buf_S, strlen(buf_S), 0);
		}
	}
}