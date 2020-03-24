#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <stdio.h>
#include<iostream>

#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")

/*
	��socket api����һ������TCP��������c-sģ�ͣ�
	���裺
		1.����һ��socket
		2.bind �����ڽ��տͻ������ӵĶ˿�
		3.listen ��������˿�
		4.accept �ȴ����ܿͻ�������
		5.send ��ͻ��˷���һ������
		6.�ر��׽��� closesocket

*/

int main_block(void)
{
	//����Windows socket 2.x����
	WORD ver = MAKEWORD(2, 2);
	WSADATA data;

	/*
		int WSAStartup(
			WORD wVersionRequined,  //����Ҫʹ�������İ汾
			LPWSADATA lpWSAData		//�ṹ��������洢������Ϣ�� ע:Windows����в�����LP��ͷ��������Ҫ�Ĳ����Ե�ַ��ʽ����
		); ��/���� �����    ��ϸ��Ϣ:https://docs.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-wsastartup
		W -> Windows
		S -> Socket
		A -> Asynchronous �첽
		����ֵ��
			0 ������ִ�гɹ�
			����᷵��error list�е�һ�����������ϸ��Ϣ���ӡ�
		ע:�������ʱ����int WSACleanup()���������;
	*/
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
	//1.����һ��SOCKET(�׽���)
	/*
		SOCKET socket(
			int af,			//ָ��Ҫ����socket��ַ�����͡� ���õ�: AF_INET -> IPv4, AF_INET6 -> IPv6
			int type,		//ָ��Ҫ�������׽������͡� ���õ�: SOCK_STREAM -> ����"��(tcp)"��, SOCK_DGRAM -> ����"���ݱ�(udp)"
			int protocol	//����ǰ����������Ҫʹ�õĴ���Э�����͡� IPPROTO_TCP -> TCP, IPPROTO_UDP -> UDP
		);����һ��SOCKET	��ϸ��Ϣ:https://docs.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-socket
		����ֵ��
			�ɹ������������׽��ֵ�������
			���򽫷���INVALID_SOCKET��ֵ�����ҿ���ͨ������WSAGetLastError�������ض��Ĵ�����롣
		ע:�������ʱ����int closesocket(SOCKET s)����׽���;
	*/
	SOCKET _serSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	//�ж�SOCKET�Ƿ񴴽��ɹ�
	if (INVALID_SOCKET == _serSock)
	{
		//��ȡ������
		int eroCode = WSAGetLastError();
		printf("ERROR(������: %d)��SOCKET����ʧ�ܣ�\n", eroCode);
		//���������
		WSACleanup();
		return 0;
	}
	//2.bind �����ڽ��տͻ������ӵĶ˿�
	/*
		int bind(
			SOCKET S,				//����󶨵�SOCKET
			const sockaddr *name,	//Ҫ��������׽��ֵı��ص�ַ��sockaddr�ṹ��ָ��
			int namelen				//����2�ĳ���
		);��SOCKET				��ϸ��Ϣ:https://docs.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-bind
		����ֵ��
			0 ����󶨳ɹ�
			���򽫷���SOCKET_ERROR��ֵ�����ҿ���ͨ������WSAGetLastError�������ض��Ĵ�����롣

	sockaddr��ϸ��Ϣ:https://docs.microsoft.com/zh-cn/windows/win32/winsock/sockaddr-2
	*/
	struct sockaddr_in _ser;
	_ser.sin_family = AF_INET;				//����
	_ser.sin_addr.S_un.S_addr = INADDR_ANY;	//���ػػ���ַ: inet_addr("127.0.0.1"); //Ҫ�󶨵�������ַ ע��INADDR_ANY -> �����ַ
	_ser.sin_port = htons(23323);			//��������ͨ�Ŷ˿�
	//�ж��Ƿ�󶨳ɹ�
	if (SOCKET_ERROR == bind(_serSock, (const struct sockaddr*)&_ser, sizeof(_ser)))
	{
		//��ȡ������
		int eroCode = WSAGetLastError();
		printf("ERROR(������: %d)��SOCKET��ʧ�ܣ�\n", eroCode);
		//����׽���
		closesocket(_serSock);
		//���������
		WSACleanup();
		return 0;
	}
	//3.listen ��������˿�
	/*
		int listen(
			SOCKET s,		//������SOCKET
			int backlog		//�������Ӷ��е���󳤶�
		);�����˿�			��ϸ��Ϣ:https://docs.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-listen
		����ֵ��
			0 ����󶨳ɹ�
			���򽫷���SOCKET_ERROR��ֵ�����ҿ���ͨ������WSAGetLastError�������ض��Ĵ�����롣
	*/
	//�жϼ����Ƿ�ɹ�
	if (SOCKET_ERROR == listen(_serSock, 5))
	{
		//��ȡ������
		int eroCode = WSAGetLastError();
		printf("ERROR(������: %d)������ʧ�ܣ�\n", eroCode);
		//����׽���
		closesocket(_serSock);
		//���������
		WSACleanup();
		return 0;
	}
	//4.accept �ȴ����տͻ�������
	/*
		SOCKET accept(
			SOCKET s,			//�����˿ڵ�SOCKET
			sockaddr *addr,
			int *addrlen		
		);�ȴ�����				��ϸ��Ϣ:https://docs.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-accept
		����ֵ��
			�ɹ������������ӵ�SOCKET�����ͻ���SOCKET
			���򷵻�INVALID_SOCKET��ֵ������ͨ������WSAGetLastError�������ض��Ĵ�����롣
	*/
	struct sockaddr_in _cli;
	int _clisize = sizeof(_cli);
	SOCKET _cliSock = accept(_serSock, (struct sockaddr *)&_cli, &_clisize);
	//�ж��Ƿ���յ�SOCKET
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
	//5.send ��ͻ��˷���һ������
	/*
		int send(
			SOCKET s,			//Ŀ��SOCKET
			const char *buf,	//���͵�����
			int len,			//���ݳ���
			int flags
		);��������				��ϸ��Ϣ:https://docs.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-send
		����ֵ��
			���ͳɹ��򷵻ط��͵��ֽ���
			���򷵻�SOCKET_ERROR��ֵ��ͨ������WSAGetLastError�������ض��Ĵ�����롣
	*/
	//�ж��Ƿ��ͳɹ�
	char buf[] = "���������ӳɹ���";
	if (SOCKET_ERROR == send(_cliSock, buf, strlen(buf), 0))
	{
		//��ȡ������
		int eroCode = WSAGetLastError();
		printf("ERROR(������: %d)�����ݷ���ʧ�ܣ�\n", eroCode);
		//����׽���
		closesocket(_serSock);
		//���������
		WSACleanup();
		return 0;
	}
	//6.�ر��׽��� closesocket
	closesocket(_serSock);
	//���������
	WSACleanup();
	return std::cin.get();
}
