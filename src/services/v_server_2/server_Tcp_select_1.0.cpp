/*
	select模型
	实现多人群聊功能
*/
#ifdef _WIN32
	#define _WINSOCK_DEPRECATED_NO_WARNINGS
	#define _CRT_SECURE_NO_WARNINGS

	#include <WinSock2.h>
	#pragma comment(lib, "ws2_32.lib")
#else
	#include <unistd.h>
	#include <arpa/inet.h>
	#include <string.h>

	#define SOCKET int
	#define INVALID_SOCKET  (SOCKET)(~0)
	#define SOCKET_ERROR            (-1)
#endif // _WIN32

#include <stdio.h>

//处理Client connection，request
int _connect(SOCKET, char*);

int main(void)
{
#ifdef _WIN32
	WORD ver = MAKEWORD(2, 2);
	WSADATA data;
	WSAStartup(ver, &data);
#endif // _WIN32

	SOCKET _serSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	struct sockaddr_in _ser;
	_ser.sin_family = AF_INET;
	_ser.sin_addr.S_un.S_addr = INADDR_ANY;
	_ser.sin_port = htons(8989);
	
	bind(_serSock, (const sockaddr*)&_ser, sizeof(_ser));

	listen(_serSock, 5);
	//创建存储所有SOCKET的列表
	fd_set sockArry;
	//清空列表
	FD_ZERO(&sockArry);
	//将服务器SOCKET放入列表
	FD_SET(_serSock, &sockArry);
	//定义初始化等待时间结构体
	struct timeval timeout;
	timeout.tv_sec = 2;
	timeout.tv_usec = 0;

	while (true)
	{
		//创建局部SOCKET数组处理有响应的SOCKET，避免影响sockArry
		fd_set ac_sock = sockArry;
		//调用select函数来实现select模型
		int s_result = select(_serSock + 1, &ac_sock, NULL, NULL, &timeout);
		//根据返回值进行逻辑处理
		if (s_result == 0)	//表示当前没有SOCKET请求
		{
			continue;
		}
		else if (s_result > 0)	//有请求活动
		{
			//遍历响应数组，依次处理各请求
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