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
#endif
#include <thread>
#include <stdio.h>

struct DataHeader
{
	int dataLength;
	int passageLength;
};

struct Data : public DataHeader
{
	Data()
	{
		dataLength = sizeof(Data);
		passageLength = strlen(Passage);
	}
	char UserName[32];
	char Passage[512];
};

//������Ϣ�̺߳���
void _message(SOCKET, Data*);
bool blocks = true;

int main(void)
{
	Data data;
	printf("�������û���:");
	scanf("%s", data.UserName);
	#ifdef _WIN32
	WORD ver = MAKEWORD(2, 2);
	WSADATA wsadata;
	WSAStartup(ver, &wsadata);
	#endif // _WIN32
	SOCKET _cliSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	struct sockaddr_in _cli;
	_cli.sin_family = AF_INET;
	#ifdef _WIN32
	_cli.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	#else
	_cli.sin_addr.s_addr = inet_addr("127.0.0.1");
	#endif // _WIN32
	_cli.sin_port = htons(8989);

	connect(_cliSock, (const sockaddr*)&_cli, sizeof(_cli));
	//����SOCKET����
	fd_set SockArry;
	FD_ZERO(&SockArry);
	FD_SET(_cliSock, &SockArry);

	struct timeval timeout;
	timeout.tv_sec = 2;
	timeout.tv_usec = 0;

	std::thread send_M(_message, _cliSock, &data);

	while (blocks)
	{
		//�����ֲ�����������Ӱ��SockArry
		fd_set ac_sock = SockArry;
		int s_result = select(_cliSock + 1, &ac_sock, NULL, NULL, &timeout);
		if (s_result == 0)
		{
			continue;
		}
		else if (s_result > 0)
		{
			//���������
			Data r_data;
			if (SOCKET_ERROR == recv(_cliSock, (char*)&r_data, sizeof(Data), 0))
			{
				printf("ERROR�����ݽ���ʧ�ܣ������µ�½��\n");
				break;
			}
			printf("%s:%s\n", r_data.UserName, r_data.Passage);
		}
	}
	send_M.join();
	#ifdef _WIN32
	//����׽���
	closesocket(_cliSock);
	//���������
	WSACleanup();
	#else
	close(_cliSock);
	#endif
	return 0;
}

//������Ϣ�߳�
void _message(SOCKET sock, Data *data)
{
	while (true)
	{
		scanf("%s", data->Passage);
		//data->passageLength = strlen(data->Passage);
		if (strcmp(data->Passage, "exit") == 0)
		{
			printf("�ͻ����ѹرգ��������!\n");
			blocks = false;
			break;
		}
		else
		{
			send(sock, (char*)data, sizeof(Data), 0);
		}
	}
}
