//根据windows特定宏判断当前所处运行环境，并载入相应头文件
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
#endif
#include <stdio.h>
/*
	简易TCP服务端
	网络模型：C--S
	消息类型：数据报（ 包头 + 数据包 ）
*/

//通过枚举定义各指令
enum  ORDER
{
	ORDER_LOG_IN,	//登入
	ORDER_LOG_OUT,	//登出
	ORDER_ERROR,	//错误
	ORDER_RESULT
};
//定义包头结构
struct DataHeader
{
	int ORDER;			//指令
	int DataLength;		//将要发送数据的长度
};
//定义ORDER_LOG_IN指令将要处理的数据包
struct LogIn :public DataHeader
{
	LogIn()
	{
		ORDER = ORDER_LOG_IN;
		DataLength = sizeof(LogIn);
	}
	char UserName[32];
	char PassWord[32];
};
//定义ORDER_LOG_OUT指令将要处理的数据包
struct LogOut :public DataHeader
{
	LogOut()
	{
		ORDER = ORDER_LOG_OUT;
		DataLength = sizeof(LogOut);
	}
	char UserName[32];
};

//定义处理请求后结果的数据包
struct LogResult :public DataHeader
{
	LogResult()
	{
		ORDER = ORDER_RESULT;
		DataLength = sizeof(LogResult);
		result = 0;//表示所执行任务正常
	}
	int result;
};

int main(void)
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
	//等待客户端连接
	sockaddr_in _cli;
	int _cliSize = sizeof(_cli);
	#ifdef _WIN32
	SOCKET _cliSock = accept(_serSock, (sockaddr*)&_cli, &_cliSize);
	#else
	SOCKET _cliSock = accept(_serSock, (sockaddr*)&_cli, (socklen_t*)&_cliSize);
	#endif
	if (INVALID_SOCKET == _cliSock)
	{
		#ifdef _WIN32
		//获取错误码
		int eroCode = WSAGetLastError();
		printf("ERROR(错误码: %d)：客户端连接失败！\n", eroCode);
		//清除套接字
		closesocket(_serSock);
		//清理网络库
		WSACleanup();
		#else
		printf("ERROR(错误码: %d)：客户端连接失败！\n", -1);
		#endif
		return 0;
	}
	else
	{
		printf("新客户端已连接！SOCKET = %d, IP = %s\n", (int)_cliSock, inet_ntoa(_cli.sin_addr));
	}
	//收发处理逻辑
	while (true)
	{
		//定义将要接收的包头
		DataHeader header = {};
		//接收包头长度的数据，并将接受的数据以包头结构存储
		int rLen = recv(_cliSock, (char *)&header, sizeof(DataHeader), 0);
		if (SOCKET_ERROR == rLen)
		{
			#ifdef _WIN32
			//获取错误码
			int eroCode = WSAGetLastError();
			printf("ERROR(错误码: %d)：数据接收失败！\n", eroCode);
			//清除套接字
			closesocket(_cliSock);
			#else
			printf("ERROR(错误码: %d)：数据接收失败！\n", -1);
			#endif
			break;
		}
		else if (rLen <= 0)
		{
			printf("客户端以退出，任务结束！\n");
			break;
		}
		else
		{
			printf("收到请求: %d, 数据长度: %d \n", header.ORDER, header.DataLength);
			//根据接收到的包头里的指令，来进行分支处理
			switch (header.ORDER)
			{
			case ORDER_LOG_IN:
			{
				//定义将要接收的数据包
				LogIn login;
				//接收相应数据包长度的数据，并将接受的数据以相应数据包结构存储
				recv(_cliSock, (char *)&login + sizeof(DataHeader), sizeof(LogIn) - sizeof(DataHeader), 0);
				/*
					//可在此处扩展登陆业务逻辑
				*/
				printf("登入请求：DataLength = %d, UserName = '%s', PassWord = '%s' \n", login.DataLength, login.UserName, login.PassWord);
				//定义将要发送的log状态
				LogResult logR;		
				send(_cliSock, (const char*)&logR, sizeof(LogResult), 0);
				break;
			}
			case ORDER_LOG_OUT:
			{
				LogOut logout;
				//因为再次之前已经接受了DataHeader长度的数据，故在此进行地址偏移，以及接收长度改变
				recv(_cliSock, (char *)&logout + sizeof(DataHeader), sizeof(LogOut) - sizeof(DataHeader), 0);
				/*
					//可在此处扩展登陆业务逻辑
				*/
				printf("登出请求：DataLength = %d, UserName = '%s' \n", logout.DataLength, logout.UserName);
				LogResult logR;		//0代表当前处于LOG_OUT状态
				send(_cliSock, (const char*)&logR, sizeof(LogResult), 0);
				break;
			}
			//接收到异常指令时
			default:
			{
				header.ORDER = ORDER_ERROR;
				header.DataLength = 0;
				send(_cliSock, (const char*)&header, sizeof(DataHeader), 0);
			}
			break;
			}
		}
	}
	#ifdef _WIN32
	closesocket(_serSock);
	WSACleanup();
	system("pause");
	#else
	close(_cliSock);
	close(_serSock);
	#endif
	return 0;
}