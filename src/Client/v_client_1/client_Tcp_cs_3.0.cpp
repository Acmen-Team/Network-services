#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <stdio.h>
#include <WinSock2.h>

#pragma comment(lib, "ws2_32.lib")

/*
	c-s�ļ�����
	�������ͣ����ݰ�
	����Э�飺�Զ���
	�ļ����Ϊ��6kb
*/

//��ͷ
struct DataHeader
{
	int dataLength;
	int fileSize;
};
//����
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
	//�������
	WORD ver = MAKEWORD(2, 2);
	WSADATA d;
	WSAStartup(ver, &d);
	//����SOCKET
	SOCKET _cliSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	struct sockaddr_in _cli;
	_cli.sin_family = AF_INET;
	_cli.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	_cli.sin_port = htons(8989);
	connect(_cliSock, (const sockaddr*)&_cli, sizeof(_cli));
	//�������ݰ�
	Data data;
	recv(_cliSock, (char*)&data, sizeof(Data), 0);
	printf("������ļ���СΪ:%d�ֽ�\n", data.fileSize);
	//���ļ�
	FILE* fp = fopen("test1.rar", "w+b");
	if (1 > fwrite(data.fileBuf, data.fileSize, 1, fp))
	{
		printf("error\n");
	}
	rewind(fp);
	fclose(fp);
	closesocket(_cliSock);
	WSACleanup();
	printf("���������ַ��˳�����:");
	getchar();
	return 0;
}