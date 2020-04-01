#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")

//����ṹ������
struct  dataPackage
{
	int _mVer;
	int _dVer;
	char serName[20];
	char serUser[20];
};

int main(void)
{
	//�������
	WORD ver = MAKEWORD(2, 2);
	WSADATA data;
	if (0 != WSAStartup(ver, &data))
	{
		int errCode = WSAGetLastError();
		printf("ERROR(�����룺%d):������ʧ��", errCode);
		return 0;
	}
	//����SOCKET
	SOCKET _cliSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == _cliSock)
	{
		int errCode = WSAGetLastError();
		printf("ERROR(�����룺%d):SOCKET����ʧ��", errCode);
		//���������
		WSACleanup();
		return 0;
	}
	//���ӷ�����
	struct sockaddr_in _cli;
	_cli.sin_family = AF_INET;
	_cli.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	_cli.sin_port = htons(33233);
	if (SOCKET_ERROR == connect(_cliSock, (const struct sockaddr*)&_cli, sizeof(_cli)))
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

	//���ͻ���
	char buf_S[1024];
	printf("------��ǰ����ָ��------\n");
	printf("getServer  ��ȡ��������Ϣ\n");
	printf("quit	   �ͻ����˳�\n");
	while (true)
	{
		char buf_R[1024] = { 0 };
		printf("��������:");
		scanf("%s", buf_S);
		if (strcmp(buf_S, "quit") == 0)
		{
			break;
		}
		if (SOCKET_ERROR == send(_cliSock, buf_S, strlen(buf_S), 0))
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
		if (SOCKET_ERROR == recv(_cliSock, buf_R, 1023, 0))
		{
			//��ȡ������
			int eroCode = WSAGetLastError();
			printf("ERROR(������: %d)�����ݽ���ʧ�ܣ�\n", eroCode);
			//����׽���
			closesocket(_cliSock);
			//���������
			WSACleanup();
			break;
		}
		dataPackage *dp = (dataPackage*)buf_R;
		printf("serName:%s serUser:%s mVer:%d dVer:%d\n", dp->serName, dp->serUser, dp->_mVer, dp->_mVer);
	}

	//����׽���
	closesocket(_cliSock);
	//���������
	WSACleanup();
	system("pause");
	return 0;
}


