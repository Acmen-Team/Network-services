#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <stdio.h>
#include<iostream>

#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")

/*
	��socket api����һ������TCP�ͻ��ˣ�c-sģ�ͣ�
	���裺
		1.����һ��socket
		2.���ӷ����� connect
		3.���շ�������Ϣ recv
		4.�ر��׽��� closesocket
*/

int main_block(void)	//����汾��ͬһvs��Ŀ�£���Ϊ�˱�������ʱmain������ͻ��block
{
	//����Windows socket 2.x����
	WORD ver = MAKEWORD(2, 2);
	WSADATA data;

	//�ж��������Ƿ�ɹ�
	if (0 != WSAStartup(ver, &data))
	{
		//��ȡ������
		int eroCode = WSAGetLastError();
		printf("ERROR(������: %d)��������ʧ�ܣ�\n", eroCode);
		//���������
		WSACleanup();
		return 0;
	}
	//1.����һ��socket
	SOCKET _cliSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	//�ж�SOCKET�Ƿ񴴽��ɹ�
	if (INVALID_SOCKET == _cliSock)
	{
		//��ȡ������
		int eroCode = WSAGetLastError();
		printf("ERROR(������: %d)��SOCKET����ʧ�ܣ�\n", eroCode);
		//���������
		WSACleanup();
		return 0;
	}
	//2.���ӷ����� connect
	/*
		int connect(
			SOCKET s,
			const sockaddr *name,
			int namelen
		);���ӷ�����			��ϸ��Ϣ:https://docs.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-connect
		����ֵ��
			0 ����󶨳ɹ�
			���򽫷���SOCKET_ERROR��ֵ�����ҿ���ͨ������WSAGetLastError�������ض��Ĵ�����롣
	*/
	struct sockaddr_in _cli;
	_cli.sin_family = AF_INET;
	_cli.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	_cli.sin_port = htons(23323);
	//�ж��Ƿ����ӳɹ�
	if (SOCKET_ERROR == connect(_cliSock, (const sockaddr*)&_cli, sizeof(_cli)))
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
	//3.���շ�������Ϣ recv
	/*
		int recv(
			SOCKET s,
			char *buf,
			int len,
			int flags
		);���շ�������Ϣ		��ϸ��Ϣ:https://docs.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-recv
		����ֵ��
			���ճɹ��򷵻ؽ��յ��ֽ���
			���򷵻�SOCKET_ERROR��ֵ��ͨ������WSAGetLastError�������ض��Ĵ�����롣
	*/
	//�ж��Ƿ���ճɹ�
	char buf[1024] = { 0 };
	if (SOCKET_ERROR == recv(_cliSock, buf, 1023, 0))
	{
		//��ȡ������
		int eroCode = WSAGetLastError();
		printf("ERROR(������: %d)�����ݽ���ʧ�ܣ�\n", eroCode);
		//����׽���
		closesocket(_cliSock);
		//���������
		WSACleanup();
		return 0;
	}
	else
	{
		printf("��������Ϣ:%s\n", buf);
	}
	//6.�ر��׽��� closesocket
	closesocket(_cliSock);
	//���������
	WSACleanup();
	return std::cin.get();
}