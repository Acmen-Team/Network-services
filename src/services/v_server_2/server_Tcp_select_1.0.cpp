#ifdef _WIN32
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <vector>
#include <algorithm>

#define SOCKET int
#define INVALID_SOCKET  (SOCKET)(~0)
#define SOCKET_ERROR            (-1)
#endif // _WIN32

#include <stdio.h>

//����Client connection��request
int _connect(SOCKET, char*);
#ifndef _WIN32
//����洢����SOCKET������
std::vector<SOCKET> clientSocks;
#endif // _WIN32

/*
	selectģ��
	ʵ�ֶ���Ⱥ�Ĺ���
*/

int main_blocks(void)
{
	#ifdef _WIN32
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
	#endif
	//����SOCKET
	SOCKET _serSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == _serSock)
	{
		#ifdef _WIN32
		int eroCode = WSAGetLastError();
		printf("ERROR(������: %d)��SOCKET����ʧ�ܣ�\n", eroCode);
		WSACleanup();
		#else
		printf("ERROR(������: %d)��SOCKET����ʧ�ܣ�\n", -1);
		#endif
		return 0;
	}
	//�󶨶˿�
	struct sockaddr_in _ser;
	_ser.sin_family = AF_INET;
	_ser.sin_port = htons(8989);
	#ifdef _WIN32
	_ser.sin_addr.S_un.S_addr = INADDR_ANY;
	#else
	_ser.sin_addr.s_addr = INADDR_ANY;
	#endif
	if (SOCKET_ERROR == bind(_serSock, (const struct sockaddr*)&_ser, sizeof(_ser)))
	{
		#ifdef _WIN32
		int eroCode = WSAGetLastError();
		printf("ERROR(������: %d)����ʧ�ܣ�\n", eroCode);
		closesocket(_serSock);
		WSACleanup();
		#else
		printf("ERROR(������: %d)����ʧ�ܣ�\n", -1);
		#endif
		return 0;
	}
	//�����˿�
	if (SOCKET_ERROR == listen(_serSock, 5))
	{
		#ifdef _WIN32
		int eroCode = WSAGetLastError();
		printf("ERROR(������: %d)������ʧ�ܣ�\n", eroCode);
		closesocket(_serSock);
		WSACleanup();
		#else
		printf("ERROR(������: %d)������ʧ�ܣ�\n", -1);
		#endif
		return 0;
	}
	//����SOCKET����
	fd_set sockArry;
	//����б�
	FD_ZERO(&sockArry);
	//��������SOCKET�����б�
	FD_SET(_serSock, &sockArry);
	#ifndef _WIN32
	//��������SOCKET����������
	clientSocks.push_back(_serSock);
	#endif // !_WIN32

	struct timeval timeout;
	timeout.tv_sec = 2;
	timeout.tv_usec = 0;

	while (true)
	{
		//�����ֲ��������Ӷ���Ӱ��sockArry
		fd_set ac_sock = sockArry;
		#ifdef _WIN32
		int s_result = select(_serSock + 1, &ac_sock, NULL, NULL, &timeout);
		#else
		//��ȡ����SOCKET�����ֵ��ΪLinuxƽ̨��select�����ṩ��Ч����
		int max = *std::max_element(clientSocks.begin(), clientSocks.end());
		//����select������ʵ��selectģ��
		int s_result = select(max + 1, &ac_sock, NULL, NULL, &timeout);
		//���ݷ���ֵ�����߼�����
		#endif // !_WIN32
		if (s_result == 0)	//��ʾ��ǰû��SOCKET����
		{
			continue;
		}
		else if (s_result > 0)	//������
		{
			//������Ӧ���飬���δ��������
			#ifdef _WIN32
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
			#else
			for (int i = 0; i < (int)clientSocks.size(); i++)
			{
				//�ж��Ƿ�Ϊ������SOCKET�������ʾ�пͻ���Ҫ���ӷ�����
				if (FD_ISSET(_serSock, &ac_sock))
				{
					struct sockaddr_in _cli;
					int clisize = sizeof(_cli);
					//�ͻ�������
					SOCKET _cliSock = accept(_serSock, (sockaddr*)&_cli, (socklen_t*)&clisize);
					if (INVALID_SOCKET == _cliSock)
					{
						#ifdef _WIN32
						printf("ERROR���ͻ�������ʧ�ܣ�\n");
						//����׽���
						closesocket(_cliSock);
						#else
						printf("ERROR���ͻ�������ʧ�ܣ�\n");
						#endif
						break;
					}
					else
					{
						printf("�¿ͻ��������ӣ�SOCKET = %d, IP = %s ", (int)_cliSock, inet_ntoa(_cli.sin_addr));
						FD_SET(_cliSock, &sockArry);
						clientSocks.push_back(_cliSock);
						char buf_S[1024] = { "SERVER:���������ӳɹ�!" };
						if (-1 == _connect(_cliSock, buf_S))
						{
							printf("�ͻ���������!\n");
							FD_CLR(_cliSock, &sockArry);
							continue;
						}
						break;
					}
				}
				//�������ʾ�ǿͻ���SOCKET����ʾ�пͻ��˷������ݣ����������н��ղ�ת��
				else if (FD_ISSET(clientSocks[i], &ac_sock))
				{
					char buf_R[1024] = { 0 };
					int rRec = recv(clientSocks[i], buf_R, 1023, 0);
					if (SOCKET_ERROR == rRec)
					{
						printf("�ͻ��� SOCKET = %d, �Ͽ�����!\n", clientSocks[i]);
						FD_CLR(clientSocks[i], &sockArry);
						continue;
					}
					else if (rRec <= 0)
					{
						printf("�ͻ��� SOCKET = %d, ������!\n", clientSocks[i]);
						FD_CLR(clientSocks[i], &sockArry);
						continue;
					}
					else
					{
						//����sockArry���飬�ѽ��յ������ݶԳ����������Լ���������������пͻ���ת��
						std::vector<SOCKET>::iterator it;
						for (it = clientSocks.begin(); it != clientSocks.end(); it++)
						{
							if (_serSock != *it && clientSocks[i] != *it)
							{
								if (-1 == _connect(*it, buf_R))
								{
									printf("�ͻ���������\n");
									FD_CLR(clientSocks[i], &sockArry);
									continue;
								}
							}
						}
					}
				}
				else
				{
					continue;
				}
			}
			#endif // _WIN32		
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