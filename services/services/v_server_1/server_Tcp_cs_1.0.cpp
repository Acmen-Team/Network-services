#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <stdio.h>
#include<iostream>

#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")

/*
	用socket api建立一个简易TCP服务器（c-s模型）
	步骤：
		1.建立一个socket
		2.bind 绑定用于接收客户端连接的端口
		3.listen 监听网络端口
		4.accept 等待接受客户端连接
		5.send 向客户端发送一条数据
		6.关闭套接字 closesocket

*/

int main_block(void)
{
	//启动Windows socket 2.x环境
	WORD ver = MAKEWORD(2, 2);
	WSADATA data;

	/*
		int WSAStartup(
			WORD wVersionRequined,  //传递要使用网络库的版本
			LPWSADATA lpWSAData		//结构体对象传至存储网络信息。 注:Windows编程中参数以LP开头代表所需要的参数以地址形式传入
		); 打开/启动 网络库    详细信息:https://docs.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-wsastartup
		W -> Windows
		S -> Socket
		A -> Asynchronous 异步
		返回值：
			0 代表函数执行成功
			否则会返回error list中的一个，详情见详细信息链接。
		注:程序结束时调用int WSACleanup()清理网络库;
	*/
	//判断网络库打开是否成功
	if (0 != WSAStartup(ver, &data))
	{
		//获取错误码
		int eroCode = WSAGetLastError();
		printf("ERROR(错误码: %d)：网络库打开失败！\n", eroCode);
		//清理网络库
		WSACleanup();
		return 0;
	}
	//1.建立一个SOCKET(套接字)
	/*
		SOCKET socket(
			int af,			//指明要创建socket地址的类型。 常用的: AF_INET -> IPv4, AF_INET6 -> IPv6
			int type,		//指明要创建的套接字类型。 常用的: SOCK_STREAM -> 面向"流(tcp)"的, SOCK_DGRAM -> 面向"数据报(udp)"
			int protocol	//基于前两个参数所要使用的传输协议类型。 IPPROTO_TCP -> TCP, IPPROTO_UDP -> UDP
		);创建一个SOCKET	详细信息:https://docs.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-socket
		返回值：
			成功返回引用新套接字的描述符
			否则将返回INVALID_SOCKET的值，并且可以通过调用WSAGetLastError来检索特定的错误代码。
		注:程序结束时调用int closesocket(SOCKET s)清除套接字;
	*/
	SOCKET _serSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	//判断SOCKET是否创建成功
	if (INVALID_SOCKET == _serSock)
	{
		//获取错误码
		int eroCode = WSAGetLastError();
		printf("ERROR(错误码: %d)：SOCKET创建失败！\n", eroCode);
		//清理网络库
		WSACleanup();
		return 0;
	}
	//2.bind 绑定用于接收客户端连接的端口
	/*
		int bind(
			SOCKET S,				//所需绑定的SOCKET
			const sockaddr *name,	//要分配给绑定套接字的本地地址的sockaddr结构的指针
			int namelen				//参数2的长度
		);绑定SOCKET				详细信息:https://docs.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-bind
		返回值：
			0 代表绑定成功
			否则将返回SOCKET_ERROR的值，并且可以通过调用WSAGetLastError来检索特定的错误代码。

	sockaddr详细信息:https://docs.microsoft.com/zh-cn/windows/win32/winsock/sockaddr-2
	*/
	struct sockaddr_in _ser;
	_ser.sin_family = AF_INET;				//类型
	_ser.sin_addr.S_un.S_addr = INADDR_ANY;	//本地回环地址: inet_addr("127.0.0.1"); //要绑定的主机地址 注：INADDR_ANY -> 任意地址
	_ser.sin_port = htons(23323);			//绑定主机的通信端口
	//判断是否绑定成功
	if (SOCKET_ERROR == bind(_serSock, (const struct sockaddr*)&_ser, sizeof(_ser)))
	{
		//获取错误码
		int eroCode = WSAGetLastError();
		printf("ERROR(错误码: %d)：SOCKET绑定失败！\n", eroCode);
		//清除套接字
		closesocket(_serSock);
		//清理网络库
		WSACleanup();
		return 0;
	}
	//3.listen 监听网络端口
	/*
		int listen(
			SOCKET s,		//监听的SOCKET
			int backlog		//挂起连接队列的最大长度
		);监听端口			详细信息:https://docs.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-listen
		返回值：
			0 代表绑定成功
			否则将返回SOCKET_ERROR的值，并且可以通过调用WSAGetLastError来检索特定的错误代码。
	*/
	//判断监听是否成功
	if (SOCKET_ERROR == listen(_serSock, 5))
	{
		//获取错误码
		int eroCode = WSAGetLastError();
		printf("ERROR(错误码: %d)：监听失败！\n", eroCode);
		//清除套接字
		closesocket(_serSock);
		//清理网络库
		WSACleanup();
		return 0;
	}
	//4.accept 等待接收客户端连接
	/*
		SOCKET accept(
			SOCKET s,			//监听端口的SOCKET
			sockaddr *addr,
			int *addrlen		
		);等待连接				详细信息:https://docs.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-accept
		返回值：
			成功返回请求连接的SOCKET，即客户端SOCKET
			否则返回INVALID_SOCKET的值，可以通过调用WSAGetLastError来检索特定的错误代码。
	*/
	struct sockaddr_in _cli;
	int _clisize = sizeof(_cli);
	SOCKET _cliSock = accept(_serSock, (struct sockaddr *)&_cli, &_clisize);
	//判读是否接收到SOCKET
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
	//5.send 向客户端发送一条数据
	/*
		int send(
			SOCKET s,			//目标SOCKET
			const char *buf,	//发送的内容
			int len,			//内容长度
			int flags
		);发送数据				详细信息:https://docs.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-send
		返回值：
			发送成功则返回发送的字节数
			否则返回SOCKET_ERROR的值，通过调用WSAGetLastError来检索特定的错误代码。
	*/
	//判断是否发送成功
	char buf[] = "服务器连接成功！";
	if (SOCKET_ERROR == send(_cliSock, buf, strlen(buf), 0))
	{
		//获取错误码
		int eroCode = WSAGetLastError();
		printf("ERROR(错误码: %d)：数据发送失败！\n", eroCode);
		//清除套接字
		closesocket(_serSock);
		//清理网络库
		WSACleanup();
		return 0;
	}
	//6.关闭套接字 closesocket
	closesocket(_serSock);
	//清理网络库
	WSACleanup();
	return std::cin.get();
}
