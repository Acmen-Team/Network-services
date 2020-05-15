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
//�����ͷ
struct DataHeader
{
	int dataLength;
	int passageLength;
};
//�������
struct Data: public DataHeader
{
	Data()
	{
		dataLength = sizeof(Data);
		passageLength = strlen(Passage);
	}
	char UserName[32];
	char Passage[512];
};

//����Client connection��request
int _connect(SOCKET, Data);
#ifndef _WIN32
//����洢����SOCKET������
std::vector<SOCKET> clientSocks;
#endif // _WIN32

int main(void)
{
	#ifdef _WIN32
	WORD ver = MAKEWORD(2, 2);
	WSADATA wsadata;
	WSAStartup(ver, &wsadata);
	#endif // _WIN32

	SOCKET _serSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	struct sockaddr_in _ser;
	_ser.sin_family = AF_INET;
	#ifdef _WIN32
	_ser.sin_addr.S_un.S_addr = ADDR_ANY;
	#else
	_ser.sin_addr.s_addr = INADDR_ANY;
	#endif // _WIN32

	_ser.sin_port = htons(8989);

	bind(_serSock, (const struct sockaddr*)&_ser, sizeof(_ser));

	listen(_serSock, 5);

	fd_set SockArry;
	FD_ZERO(&SockArry);
	FD_SET(_serSock, &SockArry);
	#ifndef _WIN32
	//��������SOCKET����������
	clientSocks.push_back(_serSock);
	#endif // !_WIN32

	struct timeval timeout;
	timeout.tv_sec = 2;
	timeout.tv_usec = 0;

	while (true)
	{
		fd_set ac_Sock = SockArry;
		#ifdef _WIN32
		int s_result = select(_serSock + 1, &ac_Sock, NULL, NULL, &timeout);
		#else
		//��ȡ����SOCKET�����ֵ��ΪLinuxƽ̨��select�����ṩ��Ч����
		int max = *std::max_element(clientSocks.begin(), clientSocks.end());
		//����select������ʵ��selectģ��
		int s_result = select(max + 1, &ac_Sock, NULL, NULL, &timeout);
		//���ݷ���ֵ�����߼�����
		#endif // _WIN32

		if (s_result == 0)
		{
			continue;
		}
		else if (s_result > 0)
		{
			//������Ӧ���飬���δ��������
			#ifdef _WIN32
			for (u_int i = 0; i < ac_Sock.fd_count; i++)
			{
				if (ac_Sock.fd_array[i] == _serSock)
				{
					struct sockaddr_in _cli;
					int clisize = sizeof(_cli);
					SOCKET _cliSock = accept(_serSock, (struct sockaddr*)&_cli, &clisize);
					printf("�¿ͻ��������ӣ�SOCKET = %d, IP = %s\n", (int)_cliSock, inet_ntoa(_cli.sin_addr));
					FD_SET(_cliSock, &SockArry);
					Data buf_s;
					strcpy(buf_s.Passage, "���������ӳɹ�!");
					strcpy(buf_s.UserName, "SERVER");
					buf_s.passageLength = strlen(buf_s.Passage);
					if (-1 == _connect(_cliSock, buf_s))
					{
						printf("�ͻ���������!\n");
						SOCKET sockTemp = ac_Sock.fd_array[i];
						FD_CLR(ac_Sock.fd_array[i], &SockArry);
						continue;
					}
				}
				else
				{
					//����洢���յ����ݵİ�����
					Data data;
					int rRec = recv(ac_Sock.fd_array[i], (char *)&data, sizeof(Data), 0);
					if (SOCKET_ERROR == rRec)
					{
						printf("�ͻ��� SOCKET = %d, �Ͽ�����!\n", ac_Sock.fd_array[i]);
						FD_CLR(ac_Sock.fd_array[i], &SockArry);
						continue;
					}
					else if (rRec <= 0)
					{
						printf("�ͻ��� SOCKET = %d, ������!\n", ac_Sock.fd_array[i]);
						FD_CLR(ac_Sock.fd_array[i], &SockArry);
						continue;
					}
					else
					{
						//����sockArry���飬�ѽ��յ������ݶԳ����������Լ���������������пͻ���ת��
						for (u_int x = 1; x < SockArry.fd_count; x++)
						{
							if (ac_Sock.fd_array[i] != SockArry.fd_array[x])
							{
								if (-1 == _connect(SockArry.fd_array[x], data))
								{
									printf("�ͻ���������\n");
									SOCKET sockTemp = ac_Sock.fd_array[i];
									FD_CLR(ac_Sock.fd_array[i], &SockArry);
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
				if (FD_ISSET(_serSock, &ac_Sock))
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
						FD_SET(_cliSock, &SockArry);
						clientSocks.push_back(_cliSock);
						Data buf_s;
						strcpy(buf_s.Passage, "���������ӳɹ�!");
						strcpy(buf_s.UserName, "SERVER");
						if (-1 == _connect(_cliSock, buf_s))
						{
							printf("�ͻ���������!\n");
							FD_CLR(_cliSock, &SockArry);
							continue;
						}
						break;
					}
				}
				//�������ʾ�ǿͻ���SOCKET����ʾ�пͻ��˷������ݣ����������н��ղ�ת��
				else if (FD_ISSET(clientSocks[i], &ac_Sock))
				{
					Data data;
					int rRec = recv(clientSocks[i], &data, sizeof(Data), 0);
					if (SOCKET_ERROR == rRec)
					{
						printf("�ͻ��� SOCKET = %d, �Ͽ�����!\n", clientSocks[i]);
						FD_CLR(clientSocks[i], &SockArry);
						continue;
					}
					else if (rRec <= 0)
					{
						printf("�ͻ��� SOCKET = %d, ������!\n", clientSocks[i]);
						FD_CLR(clientSocks[i], &SockArry);
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
								if (-1 == _connect(*it, data))
								{
									printf("�ͻ���������\n");
									FD_CLR(clientSocks[i], &SockArry);
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
			#endif
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
int _connect(SOCKET _sock, Data buf_S)
{
	if (SOCKET_ERROR == send(_sock, (char*)&buf_S, sizeof(Data), 0))
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

