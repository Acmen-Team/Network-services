#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <iostream>
//#include <Windows.h>

#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")

/*
	����TCP�����ҵ���߼����£�����������������������ͼ
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
		//��ȡ������
		int eroCode = WSAGetLastError();
		printf("ERROR(������: %d)��������ʧ�ܣ�\n", eroCode);
		//���������
		WSACleanup();
		return 0;
	}
	//����socket
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
	_ser.sin_port = htons(23324);
	//�󶨶˿�
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

	struct sockaddr_in _cli;
	int _clisize = sizeof(_cli);
	//�ȴ��ͻ�������
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
	//�����շ�������
	char buf_R[1024] = { 0 };
	char buf_S[1024] = { 0 };
	while (true)
	{
		//���տͻ�������
		int bufLen = recv(_cliSock, buf_R, 1023, 0);
		if (SOCKET_ERROR == bufLen)
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
		else if (bufLen <= 0)
		{
			printf("�ͻ������˳������������");
			break;
		}
		else
		{
			if (strcmp(buf_R, "getTime") == 0)
			{
				SYSTEMTIME sys;
				GetLocalTime(&sys);//windowsϵͳAPI����ȡ��ǰϵͳʱ��
				sprintf_s(buf_S, "%4d/%02d/%02d %02d:%02d:%02d ����%1d\n", sys.wYear, sys.wMonth, sys.wDay, sys.wHour, sys.wMinute, sys.wSecond,sys.wDayOfWeek);
			}
			else if (strcmp(buf_R, "getName") == 0)
			{
				strcpy(buf_S, "ServerName:server_Tcp_CS");
			}
			else
			{
				strcpy(buf_S, "���������������������");
			}
			//�������������
			if (SOCKET_ERROR == send(_cliSock, buf_S, strlen(buf_S), 0))
			{
				//��ȡ������
				int eroCode = WSAGetLastError();
				printf("ERROR(������: %d)�����ݷ���ʧ�ܣ�\n", eroCode);
				//����׽���
				closesocket(_cliSock);
				break;
			}
		}
	}

	//6.�ر��׽��� closesocket
	closesocket(_serSock);
	//���������
	WSACleanup();
	return 0;
}