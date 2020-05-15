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
//定义包头
struct DataHeader
{
	int dataLength;
	int passageLength;
};
//定义包体
struct Data: public DataHeader
{
	Data()
	{
		dataLength = sizeof(Data);
		passageLength = strlen(Passage);
	}
	char UserName[32];
	char Passage[512];
};

//处理Client connection，request
int _connect(SOCKET, Data);
#ifndef _WIN32
//定义存储所有SOCKET的容器
std::vector<SOCKET> clientSocks;
#endif // _WIN32

int main(void)
{
	#ifdef _WIN32
	WORD ver = MAKEWORD(2, 2);
	WSADATA wsadata;
	WSAStartup(ver, &wsadata);
	#endif // _WIN32

	SOCKET _serSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	struct sockaddr_in _ser;
	_ser.sin_family = AF_INET;
	#ifdef _WIN32
	_ser.sin_addr.S_un.S_addr = ADDR_ANY;
	#else
	_ser.sin_addr.s_addr = INADDR_ANY;
	#endif // _WIN32

	_ser.sin_port = htons(8989);

	bind(_serSock, (const struct sockaddr*)&_ser, sizeof(_ser));

	listen(_serSock, 5);

	fd_set SockArry;
	FD_ZERO(&SockArry);
	FD_SET(_serSock, &SockArry);
	#ifndef _WIN32
	//将服务器SOCKET放入容器中
	clientSocks.push_back(_serSock);
	#endif // !_WIN32

	struct timeval timeout;
	timeout.tv_sec = 2;
	timeout.tv_usec = 0;

	while (true)
	{
		fd_set ac_Sock = SockArry;
		#ifdef _WIN32
		int s_result = select(_serSock + 1, &ac_Sock, NULL, NULL, &timeout);
		#else
		//获取所有SOCKET中最大值，为Linux平台下select函数提供有效参数
		int max = *std::max_element(clientSocks.begin(), clientSocks.end());
		//调用select函数来实现select模型
		int s_result = select(max + 1, &ac_Sock, NULL, NULL, &timeout);
		//根据返回值进行逻辑处理
		#endif // _WIN32

		if (s_result == 0)
		{
			continue;
		}
		else if (s_result > 0)
		{
			//遍历响应数组，依次处理各请求
			#ifdef _WIN32
			for (u_int i = 0; i < ac_Sock.fd_count; i++)
			{
				if (ac_Sock.fd_array[i] == _serSock)
				{
					struct sockaddr_in _cli;
					int clisize = sizeof(_cli);
					SOCKET _cliSock = accept(_serSock, (struct sockaddr*)&_cli, &clisize);
					printf("新客户端已连接！SOCKET = %d, IP = %s\n", (int)_cliSock, inet_ntoa(_cli.sin_addr));
					FD_SET(_cliSock, &SockArry);
					Data buf_s;
					strcpy(buf_s.Passage, "服务器连接成功!");
					strcpy(buf_s.UserName, "SERVER");
					buf_s.passageLength = strlen(buf_s.Passage);
					if (-1 == _connect(_cliSock, buf_s))
					{
						printf("客户端已下线!\n");
						SOCKET sockTemp = ac_Sock.fd_array[i];
						FD_CLR(ac_Sock.fd_array[i], &SockArry);
						continue;
					}
				}
				else
				{
					//定义存储接收的数据的包对象
					Data data;
					int rRec = recv(ac_Sock.fd_array[i], (char *)&data, sizeof(Data), 0);
					if (SOCKET_ERROR == rRec)
					{
						printf("客户端 SOCKET = %d, 断开连接!\n", ac_Sock.fd_array[i]);
						FD_CLR(ac_Sock.fd_array[i], &SockArry);
						continue;
					}
					else if (rRec <= 0)
					{
						printf("客户端 SOCKET = %d, 已下线!\n", ac_Sock.fd_array[i]);
						FD_CLR(ac_Sock.fd_array[i], &SockArry);
						continue;
					}
					else
					{
						//遍历sockArry数组，把接收到的数据对除除服务器以及发送者以外的所有客户端转发
						for (u_int x = 1; x < SockArry.fd_count; x++)
						{
							if (ac_Sock.fd_array[i] != SockArry.fd_array[x])
							{
								if (-1 == _connect(SockArry.fd_array[x], data))
								{
									printf("客户端已下线\n");
									SOCKET sockTemp = ac_Sock.fd_array[i];
									FD_CLR(ac_Sock.fd_array[i], &SockArry);
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
				if (FD_ISSET(_serSock, &ac_Sock))
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
						FD_SET(_cliSock, &SockArry);
						clientSocks.push_back(_cliSock);
						Data buf_s;
						strcpy(buf_s.Passage, "服务器连接成功!");
						strcpy(buf_s.UserName, "SERVER");
						if (-1 == _connect(_cliSock, buf_s))
						{
							printf("客户端已下线!\n");
							FD_CLR(_cliSock, &SockArry);
							continue;
						}
						break;
					}
				}
				//否则则表示是客户端SOCKET，表示有客户端发送数据，服务器进行接收并转发
				else if (FD_ISSET(clientSocks[i], &ac_Sock))
				{
					Data data;
					int rRec = recv(clientSocks[i], &data, sizeof(Data), 0);
					if (SOCKET_ERROR == rRec)
					{
						printf("客户端 SOCKET = %d, 断开连接!\n", clientSocks[i]);
						FD_CLR(clientSocks[i], &SockArry);
						continue;
					}
					else if (rRec <= 0)
					{
						printf("客户端 SOCKET = %d, 已下线!\n", clientSocks[i]);
						FD_CLR(clientSocks[i], &SockArry);
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
								if (-1 == _connect(*it, data))
								{
									printf("客户端已下线\n");
									FD_CLR(clientSocks[i], &SockArry);
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
			#endif
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
int _connect(SOCKET _sock, Data buf_S)
{
	if (SOCKET_ERROR == send(_sock, (char*)&buf_S, sizeof(Data), 0))
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

