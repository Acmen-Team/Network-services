/*
	selectģ��
	ʵ�ֶ���Ⱥ�Ĺ���
*/
#ifdef _WIN32
	#define _WINSOCK_DEPRECATED_NO_WARNINGS
	#define _CRT_SECURE_NO_WARNINGS

	#include <WinSock2.h>
	#pragma comment(lib, "ws2_32.lib")
#else
	#include <unistd.h>
	#include <arpa/inet.h>
	#include <string.h>

	#define SOCKET int
	#define INVALID_SOCKET  (SOCKET)(~0)
	#define SOCKET_ERROR            (-1)
#endif // _WIN32

#include <stdio.h>

//����Client connection��request
int _connect(SOCKET, char*);

int main(void)
{
#ifdef _WIN32
	WORD ver = MAKEWORD(2, 2);
	WSADATA data;
	WSAStartup(ver, &data);
#endif // _WIN32

	SOCKET _serSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	struct sockaddr_in _ser;
	_ser.sin_family = AF_INET;
	_ser.sin_addr.S_un.S_addr = INADDR_ANY;
	_ser.sin_port = htons(8989);
	
	bind(_serSock, (const sockaddr*)&_ser, sizeof(_ser));

	listen(_serSock, 5);
	//�����洢����SOCKET���б�
	fd_set sockArry;
	//����б�
	FD_ZERO(&sockArry);
	//��������SOCKET�����б�
	FD_SET(_serSock, &sockArry);
	//�����ʼ���ȴ�ʱ��ṹ��
	struct timeval timeout;
	timeout.tv_sec = 2;
	timeout.tv_usec = 0;

	while (true)
	{
		//�����ֲ�SOCKET���鴦������Ӧ��SOCKET������Ӱ��sockArry
		fd_set ac_sock = sockArry;
		//����select������ʵ��selectģ��
		int s_result = select(_serSock + 1, &ac_sock, NULL, NULL, &timeout);
		//���ݷ���ֵ�����߼�����
		if (s_result == 0)	//��ʾ��ǰû��SOCKET����
		{
			continue;
		}
		else if (s_result > 0)	//������
		{
			//������Ӧ���飬���δ��������
			for (u_int i = 0; i < ac_sock.fd_count; i++)
			{
				//�ж��Ƿ�Ϊ������SOCKET�������ʾ�пͻ���Ҫ���ӷ�����
				if (ac_sock.fd_array[i] == _serSock)
				{
					struct sockaddr_in _cli;
					int clisize = sizeof(_cli);
					//�ͻ�������
					SOCKET _cliSock = accept(_serSock, (sockaddr*)&_cli, &clisize);
					if (INVALID_SOCKET == _cliSock)
					{
						#ifdef _WIN32
						printf("ERROR���ͻ�������ʧ�ܣ�\n");
						//����׽���
						closesocket(_cliSock);
						#else
						printf("ERROR���ͻ�������ʧ�ܣ�\n");
						#endif
						continue;
					}
					else
					{
						printf("�¿ͻ��������ӣ�SOCKET = %d, IP = %s ", (int)_cliSock, inet_ntoa(_cli.sin_addr));
						FD_SET(_cliSock, &sockArry);
						char buf_S[1024] = { "SERVER:���������ӳɹ�!" };
						if (-1 == _connect(_cliSock, buf_S))
						{
							printf("�ͻ���������!\n");
							SOCKET sockTemp = ac_sock.fd_array[i];
							FD_CLR(ac_sock.fd_array[i], &sockArry);
							continue;
						}
					}
				}
				//�������ʾ�ǿͻ���SOCKET����ʾ�пͻ��˷������ݣ����������н��ղ�ת��
				else
				{
					char buf_R[1024] = { 0 };
					int rRec = recv(ac_sock.fd_array[i], buf_R, 1023, 0);
					if (SOCKET_ERROR == rRec)
					{
						printf("�ͻ��� SOCKET = %d, �Ͽ�����!\n", ac_sock.fd_array[i]);
						FD_CLR(ac_sock.fd_array[i], &sockArry);
						continue;
					}
					else if (rRec <= 0)
					{
						printf("�ͻ��� SOCKET = %d, ������!\n", ac_sock.fd_array[i]);
						FD_CLR(ac_sock.fd_array[i], &sockArry);
						continue;
					}
					else
					{
						//����sockArry���飬�ѽ��յ������ݶԳ����������Լ���������������пͻ���ת��
						for (u_int x = 1; x < sockArry.fd_count; x++)
						{
							if (ac_sock.fd_array[i] != sockArry.fd_array[x])
							{
								if (-1 == _connect(sockArry.fd_array[x], buf_R))
								{
									printf("�ͻ���������\n");
									SOCKET sockTemp = ac_sock.fd_array[i];
									FD_CLR(ac_sock.fd_array[i], &sockArry);
									continue;
								}
							}
						}
					}
				}
			}
			continue;
		}
		else
		{
			continue;
		}

	}

#ifdef _WIN32
	//����׽���
	closesocket(_serSock);
	//���������
	WSACleanup();
#else
	close(_serSock);
#endif
	return 0;
}

//��ָ���ͻ��˷��;�����Ϣ
int _connect(SOCKET _sock, char *buf_S)
{
	if (SOCKET_ERROR == send(_sock, buf_S, strlen(buf_S), 0))
	{
		#ifdef _WIN32
		//��ȡ������
		int eroCode = WSAGetLastError();
		printf("ERROR(������: %d)�����ݷ���ʧ�ܣ�\n", eroCode);
		#else
		printf("ERROR(������: %d)�����ݷ���ʧ�ܣ�\n", -1);
		#endif
		return -1;
	}
	else
	{
		printf("��SOCKET = %d ת����Ϣ�ɹ�!\n", _sock);
		return 0;
	}

}