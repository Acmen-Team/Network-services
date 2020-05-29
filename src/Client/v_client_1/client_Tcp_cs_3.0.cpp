#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <stdio.h>
#include <WinSock2.h>

#pragma comment(lib, "ws2_32.lib")

/*
	c-s文件传输
	数据类型：数据包
	传输协议：自定义
	文件最大为：6kb
*/

//包头
struct DataHeader
{
	int dataLength;
	int fileSize;
};
//包体
struct Data :public DataHeader
{
	Data()
	{
		dataLength = sizeof(Data);
	}
	char fileBuf[65536];
};

int main(void)
{
	//打开网络库
	WORD ver = MAKEWORD(2, 2);
	WSADATA d;
	WSAStartup(ver, &d);
	//创建SOCKET
	SOCKET _cliSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	struct sockaddr_in _cli;
	_cli.sin_family = AF_INET;
	_cli.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	_cli.sin_port = htons(8989);
	connect(_cliSock, (const sockaddr*)&_cli, sizeof(_cli));
	//创建数据包
	Data data;
	recv(_cliSock, (char*)&data, sizeof(Data), 0);
	printf("传输的文件大小为:%d字节\n", data.fileSize);
	//打开文件
	FILE* fp = fopen("test1.rar", "w+b");
	if (1 > fwrite(data.fileBuf, data.fileSize, 1, fp))
	{
		printf("error\n");
	}
	rewind(fp);
	fclose(fp);
	closesocket(_cliSock);
	WSACleanup();
	printf("输入任意字符退出程序:");
	getchar();
	return 0;
}