#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")

/*
	����TCP�ͻ���ҵ���߼����£����ͳ������������������ͼ
	��Ϣ����:���ַ���������Ϣ
		�ŵ㣺����򵥣�������
		ȱ�㣺���ݴ�������ʱ���ַ����������Ĵ�
*/

int main_blocks(void)
{
	WORD ver = MAKEWORD(2, 2);
	WSADATA data;
	//�������
	if (0 != WSAStartup(ver, &data))
	{
		int eroCode = WSAGetLastError();
		printf("ERROR(������: %d)��������ʧ�ܣ�\n", eroCode);
		WSACleanup();
		return 0;
	}
	//����socket
	SOCKET _cliSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == _cliSock)
	{
		int eroCode = WSAGetLastError();
		printf("ERROR(������: %d)��SOCKET����ʧ�ܣ�\n", eroCode);
		WSACleanup();
		return 0;
	}
	//���ӷ�����
	struct sockaddr_in _cli;
	_cli.sin_family = AF_INET;
	_cli.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	_cli.sin_port = htons(23324);
	if (SOCKET_ERROR == connect(_cliSock, (const sockaddr*)&_cli, sizeof(_cli)))
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
	printf("getName  ��ȡ��������\n");
	printf("getTime  ��ȡ������ʱ��\n");
	while (true)
	{
		char buf_R[1024] = { 0 };
		printf("��������:");
		scanf("%s", buf_S);
		if (SOCKET_ERROR == send(_cliSock, buf_S, strlen(buf_S), 0))
		{
			//��ȡ������
			int eroCode = WSAGetLastError();
			printf("ERROR(������: %d)�����ݷ���ʧ�ܣ�\n", eroCode);
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
		printf("%s\n", buf_R);
	}

	//����׽���
	closesocket(_cliSock);
	//���������
	WSACleanup();
	return 0;
}