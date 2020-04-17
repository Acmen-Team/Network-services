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
#include <stdio.h>
/*
	����TCP�ͻ���
	����ģ�ͣ�C--S
	��Ϣ���ͣ����ݰ�
*/

//ͨ��ö�ٶ����ָ��
enum  ORDER
{
	ORDER_LOG_IN,	//����
	ORDER_LOG_OUT,	//�ǳ�
	ORDER_ERROR,	//����
	ORDER_RESULT
};
//�����ͷ�ṹ
struct DataHeader
{
	int ORDER;			//ָ��
	int DataLength;		//��Ҫ�������ݵĳ���
};
//����ORDER_LOG_INָ�Ҫ��������ݰ�
struct LogIn :public DataHeader
{
	LogIn()
	{
		ORDER = ORDER_LOG_IN;
		DataLength = sizeof(LogIn);
	}
	char UserName[32];
	char PassWord[32];
};
//����ORDER_LOG_OUTָ�Ҫ��������ݰ�
struct LogOut :public DataHeader
{
	LogOut()
	{
		ORDER = ORDER_LOG_OUT;
		DataLength = sizeof(LogOut);
	}
	char UserName[32];
};

//���崦��������������ݰ�
struct LogResult :public DataHeader
{
	LogResult()
	{
		ORDER = ORDER_RESULT;
		DataLength = sizeof(LogResult);
		result = 0;//��ʾ��ִ����������
	}
	int result;
};

int main_block(void)
{
	#ifdef _WIN32
	WORD ver = MAKEWORD(2, 2);
	WSADATA data;
	if (0 != WSAStartup(ver, &data))
	{
		int errCode = WSAGetLastError();
		printf("ERROR(�����룺%d):������ʧ��", errCode);
		return 0;
	}
	#endif
	SOCKET _cliSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == _cliSock)
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

	struct sockaddr_in _cli;
	_cli.sin_family = AF_INET;
	_cli.sin_port = htons(8989);
	#ifdef _WIN32
	_cli.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	#else
	_cli.sin_addr.s_addr = inet_addr("127.0.0.1");
	#endif
	if (SOCKET_ERROR == connect(_cliSock, (const struct sockaddr*)&_cli, sizeof(_cli)))
	{
		#ifdef _WIN32
		//��ȡ������
		int eroCode = WSAGetLastError();
		printf("ERROR(������: %d)������������ʧ�ܣ�\n", eroCode);
		//����׽���
		closesocket(_cliSock);
		//���������
		WSACleanup();
		#else
		printf("ERROR(������: %d)������������ʧ�ܣ�\n", -1);
		#endif
		return 0;
	}

	//������մ洢�û�������ڴ�
	char buf_S[32];
	printf("------��ǰ����ָ��------\n");
	printf("login	�û�����\n");
	printf("logout	�û��ǳ�\n");
	printf("quit	�ͻ����˳�\n");
	//�շ�����
	while (true)
	{
		printf("��������:");
		scanf("%s", buf_S);
		//�����û�����
		if (strcmp(buf_S, "quit") == 0)
		{
			break;
		}
		else if (strcmp(buf_S, "login") == 0)
		{
			//���岢��ʼ����Ҫ���͵����ݰ�
			LogIn login;
			strcpy(login.UserName, "Holy-YxY");
			strcpy(login.PassWord, "YxY");
			//�������ݰ�
			send(_cliSock, (const char*)&login, sizeof(login), 0);
			//���彫Ҫ���ܵ�������
			LogResult logR;
			//���ս��
			recv(_cliSock, (char*)&logR, sizeof(LogResult), 0);
			printf("LogResult = %d\n", logR.result);
		}
		else if (strcmp(buf_S, "logout") == 0)
		{
			LogOut logout;
			strcpy(logout.UserName, "Holy-YxY");;
			send(_cliSock, (const char*)&logout, sizeof(logout), 0);
			LogResult logR;
			recv(_cliSock, (char*)&logR, sizeof(LogResult), 0);
			printf("LogResult = %d\n", logR.result);
		}
		//���û�������пͻ��˲���У�飬������Ч������������
		else
		{
			printf("������Ч�����������룡\n");
		}
	}
	#ifdef _WIN32
	//����׽���
	closesocket(_cliSock);
	//���������
	WSACleanup();
	system("pause");
	#else
	close(_cliSock);
	#endif
	return 0;
}