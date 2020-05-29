#define _CRT_SECURE_NO_WARNINGS

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
	int dataLength; //数据包总长度
	int fileSize;	//文件大小
};
//包体
struct Data:public DataHeader
{
	Data()
	{
		dataLength = sizeof(Data);
	}
	char fileBuf[65536];
};


int main(void)
{
	char fileName[64];
	//打开网络库
	WORD ver = MAKEWORD(2, 2);
	WSADATA d;
	WSAStartup(ver, &d);
	//创建SOCKET
	SOCKET _serSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	//绑定端口
	struct sockaddr_in _ser;
	_ser.sin_family = AF_INET;
	_ser.sin_addr.S_un.S_addr = INADDR_ANY;
	_ser.sin_port = htons(8989);
	bind(_serSock, (const struct sockaddr*)&_ser, sizeof(_ser));
	//监听端口
	listen(_serSock, 5);
	//等待连接
	struct sockaddr_in _cli;
	int size = sizeof(_cli);
	SOCKET _cliSock = accept(_serSock, (sockaddr*)&_cli, &size);
	//创建数据包
	Data data;
	//打开文件
	FILE* fp = fopen("test.rar", "r+b");
	//偏移文件指针至末尾
	fseek(fp, 0, SEEK_END);
	//获取文件长度
	data.fileSize = ftell(fp);
	printf("传输的文件大小为:%d字节\n", data.fileSize);
	//将文件指针重新指向文件开头
	fseek(fp, 0, SEEK_SET);
	fread(data.fileBuf, data.fileSize, 1, fp);

	send(_cliSock, (char*)&data, sizeof(Data), 0);
	fclose(fp);
	closesocket(_serSock);
	closesocket(_cliSock);
	WSACleanup();
	printf("输入任意字符退出程序:");
	getchar();
	return 0;
}