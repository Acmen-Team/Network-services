#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")

/*
	����TCP�����
	����ģ�ͣ�C--S
	��Ϣ���ͣ����ݰ�
*/

//ͨ��ö�ٶ����ָ��
enum  ORDER
{
	ORDER_LOG_IN,	//����
	ORDER_LOG_OUT,	//�ǳ�
	ORDER_ERROR		//����
};
//�����ͷ�ṹ
struct DataHeader
{
	int ORDER;			//ָ��
	int DataLength;		//��Ҫ�������ݵĳ���
};
//����ORDER_LOG_INָ�Ҫ��������ݰ�
struct LogIn
{
	char UserName[32];
	char PassWord[32];
};
//����ORDER_LOG_OUTָ�Ҫ��������ݰ�
struct LogOut
{
	char UserName[32];
};
//���崦��������������ݰ�
struct LogResult
{
	int result;		//log״̬
};

int main(void)
{

	WORD ver = MAKEWORD(2, 2);
	WSADATA data;
	if (0 != WSAStartup(ver, &data))
	{
		int errCode = WSAGetLastError();
		printf("ERROR(�����룺%d):������ʧ��", errCode);
		return 0;
	}

	SOCKET _cliSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == _cliSock)
	{
		int errCode = WSAGetLastError();
		printf("ERROR(�����룺%d):SOCKET����ʧ��", errCode);
		//���������
		WSACleanup();
		return 0;
	}

	struct sockaddr_in _cli;
	_cli.sin_family = AF_INET;
	_cli.sin_port = htons(32123);
	_cli.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
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

	//������մ洢�û�������ڴ�
	char buf_S[32];
	printf("------��ǰ����ָ��------\n");
	printf("login	�û�����\n");
	printf("logout	�û��ȳ�\n");
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
			LogIn login = { "Holy-YxY", "YxY" };
			//���岢�������ݰ��԰�ͷ���и���
			DataHeader header;
			header.ORDER = ORDER_LOG_IN;	//�����Ӧ��Ӧָ��
			header.DataLength = sizeof(login);	//��Ӧָ�Ҫ���͵����ݰ�����
			//���Ͱ�ͷ
			send(_cliSock, (const char*)&header, sizeof(header), 0);
			//�������ݰ�
			send(_cliSock, (const char*)&login, sizeof(login), 0);
			//���հ�ͷ
			recv(_cliSock, (char*)&header, sizeof(DataHeader), 0);
			//���彫Ҫ���ܵ�������
			LogResult logR = {  };
			//���ս��
			recv(_cliSock, (char*)&logR, sizeof(LogResult), 0);
			printf("LogResult = %d\n", logR.result);
		}
		else if (strcmp(buf_S, "logout") == 0)
		{
			LogOut logout = { "Holy-YxY" };
			DataHeader header;
			header.ORDER = ORDER_LOG_OUT;
			header.DataLength = sizeof(logout);
			send(_cliSock, (const char*)&header, sizeof(header), 0);
			send(_cliSock, (const char*)&logout, sizeof(logout), 0);
			recv(_cliSock, (char*)&header, sizeof(DataHeader), 0);
			LogResult logR = {  };
			recv(_cliSock, (char*)&logR, sizeof(LogResult), 0);
			printf("LogResult = %d\n", logR.result);
		}
		//���û�������пͻ��˲���У�飬������Ч������������
		else
		{
			printf("������Ч�����������룡\n");
		}
	}
	//����׽���
	closesocket(_cliSock);
	//���������
	WSACleanup();
	system("pause");
	return 0;
}