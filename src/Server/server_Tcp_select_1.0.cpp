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
#include <algorithm>

#define SOCKET int
#define INVALID_SOCKET  (SOCKET)(~0)
#define SOCKET_ERROR            (-1)
#endif // _WIN32

#include <stdio.h>

//处理Client connection，request
int _connect(SOCKET, char*);
#ifndef _WIN32
//定义存储所有SOCKET的容器
std::vector<SOCKET> clientSocks;
#endif // _WIN32

/*
	select模型
	实现多人群聊功能
*/

int main_blocks(void)
{
	#ifdef _WIN32
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
	#endif
	//创建SOCKET
	SOCKET _serSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == _serSock)
	{
		#ifdef _WIN32
		int eroCode = WSAGetLastError();
		printf("ERROR(错误码: %d)：SOCKET创建失败！\n", eroCode);
		WSACleanup();
		#else
		printf("ERROR(错误码: %d)：SOCKET创建失败！\n", -1);
		#endif
		return 0;
	}
	//绑定端口
	struct sockaddr_in _ser;
	_ser.sin_family = AF_INET;
	_ser.sin_port = htons(8989);
	#ifdef _WIN32
	_ser.sin_addr.S_un.S_addr = INADDR_ANY;
	#else
	_ser.sin_addr.s_addr = INADDR_ANY;
	#endif
	if (SOCKET_ERROR == bind(_serSock, (const struct sockaddr*)&_ser, sizeof(_ser)))
	{
		#ifdef _WIN32
		int eroCode = WSAGetLastError();
		printf("ERROR(错误码: %d)：绑定失败！\n", eroCode);
		closesocket(_serSock);
		WSACleanup();
		#else
		printf("ERROR(错误码: %d)：绑定失败！\n", -1);
		#endif
		return 0;
	}
	//监听端口
	if (SOCKET_ERROR == listen(_serSock, 5))
	{
		#ifdef _WIN32
		int eroCode = WSAGetLastError();
		printf("ERROR(错误码: %d)：监听失败！\n", eroCode);
		closesocket(_serSock);
		WSACleanup();
		#else
		printf("ERROR(错误码: %d)：监听失败！\n", -1);
		#endif
		return 0;
	}
	//创建SOCKET数组
	fd_set sockArry;
	//清空列表
	FD_ZERO(&sockArry);
	//将服务器SOCKET放入列表
	FD_SET(_serSock, &sockArry);
	#ifndef _WIN32
	//将服务器SOCKET放入容器中
	clientSocks.push_back(_serSock);
	#endif // !_WIN32

	struct timeval timeout;
	timeout.tv_sec = 2;
	timeout.tv_usec = 0;

	while (true)
	{
		//创建局部变量，从而不影响sockArry
		fd_set ac_sock = sockArry;
		#ifdef _WIN32
		int s_result = select(_serSock + 1, &ac_sock, NULL, NULL, &timeout);
		#else
		//获取所有SOCKET中最大值，为Linux平台下select函数提供有效参数
		int max = *std::max_element(clientSocks.begin(), clientSocks.end());
		//调用select函数来实现select模型
		int s_result = select(max + 1, &ac_sock, NULL, NULL, &timeout);
		//根据返回值进行逻辑处理
		#endif // !_WIN32
		if (s_result == 0)	//表示当前没有SOCKET请求
		{
			continue;
		}
		else if (s_result > 0)	//有请求活动
		{
			//遍历响应数组，依次处理各请求
			#ifdef _WIN32
			for (u_int i = 0; i < ac_sock.fd_count; i++)
			{
				//判断是否为服务器SOCKET，是则表示有客户端要连接服务器
				if (ac_sock.fd_array[i] == _serSock)
				{
					struct sockaddr_in _cli;
					int clisize = sizeof(_cli);
					//客户端连接
					SOCKET _cliSock = accept(_serSock, (sockaddr*)&_cli, &clisize);
					if (INVALID_SOCKET == _cliSock)
					{
						#ifdef _WIN32
						printf("ERROR：客户端连接失败！\n");
						//清除套接字
						closesocket(_cliSock);
						#else
						printf("ERROR：客户端连接失败！\n");
						#endif
						continue;
					}
					else
					{
						printf("新客户端已连接！SOCKET = %d, IP = %s ", (int)_cliSock, inet_ntoa(_cli.sin_addr));
						FD_SET(_cliSock, &sockArry);
						char buf_S[1024] = { "SERVER:服务器连接成功!" };
						if (-1 == _connect(_cliSock, buf_S))
						{
							printf("客户端已下线!\n");
							SOCKET sockTemp = ac_sock.fd_array[i];
							FD_CLR(ac_sock.fd_array[i], &sockArry);
							continue;
						}
					}
				}
				//否则则表示是客户端SOCKET，表示有客户端发送数据，服务器进行接收并转发
				else
				{
					char buf_R[1024] = { 0 };
					int rRec = recv(ac_sock.fd_array[i], buf_R, 1023, 0);
					if (SOCKET_ERROR == rRec)
					{
						printf("客户端 SOCKET = %d, 断开连接!\n", ac_sock.fd_array[i]);
						FD_CLR(ac_sock.fd_array[i], &sockArry);
						continue;
					}
					else if (rRec <= 0)
					{
						printf("客户端 SOCKET = %d, 已下线!\n", ac_sock.fd_array[i]);
						FD_CLR(ac_sock.fd_array[i], &sockArry);
						continue;
					}
					else
					{
						//遍历sockArry数组，把接收到的数据对除除服务器以及发送者以外的所有客户端转发
						for (u_int x = 1; x < sockArry.fd_count; x++)
						{
							if (ac_sock.fd_array[i] != sockArry.fd_array[x])
							{
								if (-1 == _connect(sockArry.fd_array[x], buf_R))
								{
									printf("客户端已下线\n");
									SOCKET sockTemp = ac_sock.fd_array[i];
									FD_CLR(ac_sock.fd_array[i], &sockArry);
									continue;
								}
							}
						}
					}
				}
			}
			#else
			for (int i = 0; i < (int)clientSocks.size(); i++)
			{
				//判断是否为服务器SOCKET，是则表示有客户端要连接服务器
				if (FD_ISSET(_serSock, &ac_sock))
				{
					struct sockaddr_in _cli;
					int clisize = sizeof(_cli);
					//客户端连接
					SOCKET _cliSock = accept(_serSock, (sockaddr*)&_cli, (socklen_t*)&clisize);
					if (INVALID_SOCKET == _cliSock)
					{
						#ifdef _WIN32
						printf("ERROR：客户端连接失败！\n");
						//清除套接字
						closesocket(_cliSock);
						#else
						printf("ERROR：客户端连接失败！\n");
						#endif
						break;
					}
					else
					{
						printf("新客户端已连接！SOCKET = %d, IP = %s ", (int)_cliSock, inet_ntoa(_cli.sin_addr));
						FD_SET(_cliSock, &sockArry);
						clientSocks.push_back(_cliSock);
						char buf_S[1024] = { "SERVER:服务器连接成功!" };
						if (-1 == _connect(_cliSock, buf_S))
						{
							printf("客户端已下线!\n");
							FD_CLR(_cliSock, &sockArry);
							continue;
						}
						break;
					}
				}
				//否则则表示是客户端SOCKET，表示有客户端发送数据，服务器进行接收并转发
				else if (FD_ISSET(clientSocks[i], &ac_sock))
				{
					char buf_R[1024] = { 0 };
					int rRec = recv(clientSocks[i], buf_R, 1023, 0);
					if (SOCKET_ERROR == rRec)
					{
						printf("客户端 SOCKET = %d, 断开连接!\n", clientSocks[i]);
						FD_CLR(clientSocks[i], &sockArry);
						continue;
					}
					else if (rRec <= 0)
					{
						printf("客户端 SOCKET = %d, 已下线!\n", clientSocks[i]);
						FD_CLR(clientSocks[i], &sockArry);
						continue;
					}
					else
					{
						//遍历sockArry数组，把接收到的数据对除除服务器以及发送者以外的所有客户端转发
						std::vector<SOCKET>::iterator it;
						for (it = clientSocks.begin(); it != clientSocks.end(); it++)
						{
							if (_serSock != *it && clientSocks[i] != *it)
							{
								if (-1 == _connect(*it, buf_R))
								{
									printf("客户端已下线\n");
									FD_CLR(clientSocks[i], &sockArry);
									continue;
								}
							}
						}
					}
				}
				else
				{
					continue;
				}
			}
			#endif // _WIN32		
			continue;
		}
		else
		{
			continue;
		}

	}

	#ifdef _WIN32
	//清除套接字
	closesocket(_serSock);
	//清理网络库
	WSACleanup();
	#else
	close(_serSock);
	#endif
	return 0;
}

//向指定客户端发送具体消息
int _connect(SOCKET _sock, char *buf_S)
{
	if (SOCKET_ERROR == send(_sock, buf_S, strlen(buf_S), 0))
	{
		#ifdef _WIN32
		//获取错误码
		int eroCode = WSAGetLastError();
		printf("ERROR(错误码: %d)：数据发送失败！\n", eroCode);
		#else
		printf("ERROR(错误码: %d)：数据发送失败！\n", -1);
		#endif
		return -1;
	}
	else
	{
		printf("向SOCKET = %d 转发消息成功!\n", _sock);
		return 0;
	}

}