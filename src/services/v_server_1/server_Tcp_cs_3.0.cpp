#define _CRT_SECURE_NO_WARNINGS

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
	int dataLength; //���ݰ��ܳ���
	int fileSize;	//�ļ���С
};
//����
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
	//�������
	WORD ver = MAKEWORD(2, 2);
	WSADATA d;
	WSAStartup(ver, &d);
	//����SOCKET
	SOCKET _serSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	//�󶨶˿�
	struct sockaddr_in _ser;
	_ser.sin_family = AF_INET;
	_ser.sin_addr.S_un.S_addr = INADDR_ANY;
	_ser.sin_port = htons(8989);
	bind(_serSock, (const struct sockaddr*)&_ser, sizeof(_ser));
	//�����˿�
	listen(_serSock, 5);
	//�ȴ�����
	struct sockaddr_in _cli;
	int size = sizeof(_cli);
	SOCKET _cliSock = accept(_serSock, (sockaddr*)&_cli, &size);
	//�������ݰ�
	Data data;
	//���ļ�
	FILE* fp = fopen("test.rar", "r+b");
	//ƫ���ļ�ָ����ĩβ
	fseek(fp, 0, SEEK_END);
	//��ȡ�ļ�����
	data.fileSize = ftell(fp);
	printf("������ļ���СΪ:%d�ֽ�\n", data.fileSize);
	//���ļ�ָ������ָ���ļ���ͷ
	fseek(fp, 0, SEEK_SET);
	fread(data.fileBuf, data.fileSize, 1, fp);

	send(_cliSock, (char*)&data, sizeof(Data), 0);
	fclose(fp);
	closesocket(_serSock);
	closesocket(_cliSock);
	WSACleanup();
	printf("���������ַ��˳�����:");
	getchar();
	return 0;
}