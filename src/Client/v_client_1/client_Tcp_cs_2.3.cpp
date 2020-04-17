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
	简易TCP客户端
	网络模型：C--S
	消息类型：数据包
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

int main_block(void)
{
	#ifdef _WIN32
	WORD ver = MAKEWORD(2, 2);
	WSADATA data;
	if (0 != WSAStartup(ver, &data))
	{
		int errCode = WSAGetLastError();
		printf("ERROR(错误码：%d):网络库打开失败", errCode);
		return 0;
	}
	#endif
	SOCKET _cliSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == _cliSock)
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

	struct sockaddr_in _cli;
	_cli.sin_family = AF_INET;
	_cli.sin_port = htons(8989);
	#ifdef _WIN32
	_cli.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	#else
	_cli.sin_addr.s_addr = inet_addr("127.0.0.1");
	#endif
	if (SOCKET_ERROR == connect(_cliSock, (const struct sockaddr*)&_cli, sizeof(_cli)))
	{
		#ifdef _WIN32
		//获取错误码
		int eroCode = WSAGetLastError();
		printf("ERROR(错误码: %d)：服务器连接失败！\n", eroCode);
		//清除套接字
		closesocket(_cliSock);
		//清理网络库
		WSACleanup();
		#else
		printf("ERROR(错误码: %d)：服务器连接失败！\n", -1);
		#endif
		return 0;
	}

	//定义接收存储用户输入的内存
	char buf_S[32];
	printf("------当前可用指令------\n");
	printf("login	用户登入\n");
	printf("logout	用户登出\n");
	printf("quit	客户端退出\n");
	//收发机制
	while (true)
	{
		printf("调用请求:");
		scanf("%s", buf_S);
		//接收用户输入
		if (strcmp(buf_S, "quit") == 0)
		{
			break;
		}
		else if (strcmp(buf_S, "login") == 0)
		{
			//定义并初始化将要发送的数据包
			LogIn login;
			strcpy(login.UserName, "Holy-YxY");
			strcpy(login.PassWord, "YxY");
			//发送数据包
			send(_cliSock, (const char*)&login, sizeof(login), 0);
			//定义将要接受的请求结果
			LogResult logR;
			//接收结果
			recv(_cliSock, (char*)&logR, sizeof(LogResult), 0);
			printf("LogResult = %d\n", logR.result);
		}
		else if (strcmp(buf_S, "logout") == 0)
		{
			LogOut logout;
			strcpy(logout.UserName, "Holy-YxY");;
			send(_cliSock, (const char*)&logout, sizeof(logout), 0);
			LogResult logR;
			recv(_cliSock, (char*)&logR, sizeof(LogResult), 0);
			printf("LogResult = %d\n", logR.result);
		}
		//对用户输入进行客户端层面校验，避免无效请求发往服务器
		else
		{
			printf("请求无效，请重新输入！\n");
		}
	}
	#ifdef _WIN32
	//清除套接字
	closesocket(_cliSock);
	//清理网络库
	WSACleanup();
	system("pause");
	#else
	close(_cliSock);
	#endif
	return 0;
}