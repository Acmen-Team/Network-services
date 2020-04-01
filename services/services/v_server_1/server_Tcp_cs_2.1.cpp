#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")
using namespace std;

/*
	����TCP�����ҵ���߼����£�����������������������ͼ
	��Ϣ����:�ṹ������
*/

//����ṹ������
struct dataPackage
{
	int _mVer;
	int _dVer;
	char serName[20];
	char serUser[20];
};

int main()
{
	//�������
	WORD ver = MAKEWORD(2, 2);
	WSADATA datd;
	if (0 != WSAStartup(ver, &datd))
	{
		int eroCode = WSAGetLastError();
		printf("ERROR(������: %d)��������ʧ�ܣ�\n", eroCode);
		WSACleanup();
		return 0;
	}
	//����SOCKET
	SOCKET _serSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == _serSock)
	{
		int eroCode = WSAGetLastError();
		printf("ERROR(������: %d)��SOCKET����ʧ�ܣ�\n", eroCode);
		WSACleanup();
		return 0;
	}
	//�󶨶˿�
	struct sockaddr_in _ser;
	_ser.sin_family = AF_INET;
	_ser.sin_addr.S_un.S_addr = INADDR_ANY;
	_ser.sin_port = htons(33233);
	if (SOCKET_ERROR == bind(_serSock, (const struct sockaddr*)&_ser, sizeof(_ser)))
	{
		int eroCode = WSAGetLastError();
		printf("ERROR(������: %d)��SOCKET��ʧ�ܣ�\n", eroCode);
		closesocket(_serSock);
		WSACleanup();
		return 0;
	}
	//�����˿�
	if (SOCKET_ERROR == listen(_serSock, 5))
	{
		int eroCode = WSAGetLastError();
		printf("ERROR(������: %d)������ʧ�ܣ�\n", eroCode);
		closesocket(_serSock);
		WSACleanup();
		return 0;
	}
	//�ȴ��ͻ�������
	struct sockaddr_in _cli;
	int _clisize = sizeof(_cli);
	SOCKET _cliSock = accept(_serSock, (struct sockaddr*)&_cli, &_clisize);
	if (INVALID_SOCKET == _cliSock)
	{
		//��ȡ������
		int eroCode = WSAGetLastError();
		printf("ERROR(������: %d)���ͻ�������ʧ�ܣ�\n", eroCode);
		//����׽���
		closesocket(_serSock);
		//���������
		WSACleanup();
		return 0;
	}
	else
	{
		printf("�¿ͻ��������ӣ� IP = %s\n", inet_ntoa(_cli.sin_addr));
	}
	//�������
	char buf_R[1024] = { 0 };
	dataPackage dp = { 2, 1, "server_Tcp_cs", "Holy-YxY" };
	while (true)
	{
		int bufLen = recv(_cliSock, buf_R, 1023, 0);
		if (SOCKET_ERROR == bufLen)
		{
			//��ȡ������
			int eroCode = WSAGetLastError();
			printf("ERROR(������: %d)�����ݽ���ʧ�ܣ�\n", eroCode);
			//����׽���
			closesocket(_cliSock);
			break;
		}
		else if (bufLen <= 0)
		{
			printf("�ͻ������˳������������\n");
			//����׽���
			closesocket(_cliSock);
			break;
		}
		else
		{
			if (strcmp(buf_R, "getServer") == 0)
			{
				if (SOCKET_ERROR == send(_cliSock, (const char*)&dp, sizeof(dataPackage), 0))
				{
					//��ȡ������
					int eroCode = WSAGetLastError();
					printf("ERROR(������: %d)������������ʧ�ܣ�\n", eroCode);
					//����׽���
					closesocket(_cliSock);
					//���������
					WSACleanup();
					return 0;
				}
			}
		}
	}

	//����׽���
	closesocket(_serSock);
	//���������
	WSACleanup();
	system("pause");
	return 0;
}