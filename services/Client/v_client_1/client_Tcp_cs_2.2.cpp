#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")

/*
	简易TCP服务端
	网络模型：C--S
	消息类型：数据包
*/

//通过枚举定义各指令
enum  ORDER
{
	ORDER_LOG_IN,	//登入
	ORDER_LOG_OUT,	//登出
	ORDER_ERROR		//错误
};
//定义包头结构
struct DataHeader
{
	int ORDER;			//指令
	int DataLength;		//将要发送数据的长度
};
//定义ORDER_LOG_IN指令将要处理的数据包
struct LogIn
{
	char UserName[32];
	char PassWord[32];
};
//定义ORDER_LOG_OUT指令将要处理的数据包
struct LogOut
{
	char UserName[32];
};
//定义处理请求后结果的数据包
struct LogResult
{
	int result;		//log状态
};

int main(void)
{

	WORD ver = MAKEWORD(2, 2);
	WSADATA data;
	if (0 != WSAStartup(ver, &data))
	{
		int errCode = WSAGetLastError();
		printf("ERROR(错误码：%d):网络库打开失败", errCode);
		return 0;
	}

	SOCKET _cliSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == _cliSock)
	{
		int errCode = WSAGetLastError();
		printf("ERROR(错误码：%d):SOCKET创建失败", errCode);
		//清理网络库
		WSACleanup();
		return 0;
	}

	struct sockaddr_in _cli;
	_cli.sin_family = AF_INET;
	_cli.sin_port = htons(32123);
	_cli.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
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

	//定义接收存储用户输入的内存
	char buf_S[32];
	printf("------当前可用指令------\n");
	printf("login	用户登入\n");
	printf("logout	用户等出\n");
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
			LogIn login = { "Holy-YxY", "YxY" };
			//定义并根据数据包对包头进行复制
			DataHeader header;
			header.ORDER = ORDER_LOG_IN;	//请求对应相应指令
			header.DataLength = sizeof(login);	//相应指令将要发送的数据包长度
			//发送包头
			send(_cliSock, (const char*)&header, sizeof(header), 0);
			//发送数据包
			send(_cliSock, (const char*)&login, sizeof(login), 0);
			//接收包头
			recv(_cliSock, (char*)&header, sizeof(DataHeader), 0);
			//定义将要接受的请求结果
			LogResult logR = {  };
			//接收结果
			recv(_cliSock, (char*)&logR, sizeof(LogResult), 0);
			printf("LogResult = %d\n", logR.result);
		}
		else if (strcmp(buf_S, "logout") == 0)
		{
			LogOut logout = { "Holy-YxY" };
			DataHeader header;
			header.ORDER = ORDER_LOG_OUT;
			header.DataLength = sizeof(logout);
			send(_cliSock, (const char*)&header, sizeof(header), 0);
			send(_cliSock, (const char*)&logout, sizeof(logout), 0);
			recv(_cliSock, (char*)&header, sizeof(DataHeader), 0);
			LogResult logR = {  };
			recv(_cliSock, (char*)&logR, sizeof(LogResult), 0);
			printf("LogResult = %d\n", logR.result);
		}
		//对用户输入进行客户端层面校验，避免无效请求发往服务器
		else
		{
			printf("请求无效，请重新输入！\n");
		}
	}
	//清除套接字
	closesocket(_cliSock);
	//清理网络库
	WSACleanup();
	system("pause");
	return 0;
}