#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")

/*
	����TCP�����
	����ģ�ͣ�C--S
	��Ϣ���ͣ����ݱ��� ��ͷ + ���ݰ� ��
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
	//�������
	if (0 != WSAStartup(ver, &data))
	{
		int eroCode = WSAGetLastError();
		printf("ERROR(������: %d)��������ʧ�ܣ�\n", eroCode);
		WSACleanup();
		return 0;
	}
	//����SOCKET
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
	_ser.sin_port = htons(32123);
	_ser.sin_addr.S_un.S_addr = INADDR_ANY;
	if (SOCKET_ERROR == bind(_serSock, (const struct sockaddr*)&_ser, sizeof(_ser)))
	{
		int eroCode = WSAGetLastError();
		printf("ERROR(������: %d)����ʧ�ܣ�\n", eroCode);
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
	//�ȴ��ͻ�������
	sockaddr_in _cli;
	int _cliSize = sizeof(_cli);
	SOCKET _cliSock = accept(_serSock, (sockaddr*)&_cli, &_cliSize);
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
		printf("�¿ͻ��������ӣ�SOCKET = %d, IP = %s\n", (int)_cliSock,inet_ntoa(_cli.sin_addr));
	}
	//�շ������߼�
	while (true)
	{
		//���彫Ҫ���յİ�ͷ
		DataHeader header = {};
		//���հ�ͷ���ȵ����ݣ��������ܵ������԰�ͷ�ṹ�洢
		int rLen = recv(_cliSock, (char *)&header, sizeof(DataHeader), 0);
		if (SOCKET_ERROR == rLen)
		{
			//��ȡ������
			int eroCode = WSAGetLastError();
			printf("ERROR(������: %d)�����ݽ���ʧ�ܣ�\n", eroCode);
			//����׽���
			closesocket(_cliSock);
			break;
		}
		else if (rLen <= 0)
		{
			printf("�ͻ������˳������������\n");
			//����׽���
			closesocket(_cliSock);
			break;
		}
		else
		{
			printf("�յ�����: %d, ���ݳ���: %d \n", header.ORDER, header.DataLength);
			//���ݽ��յ��İ�ͷ���ָ������з�֧����
			switch (header.ORDER)
			{
			case ORDER_LOG_IN:
			{
				//���彫Ҫ���յ����ݰ�
				LogIn login = {};
				//������Ӧ���ݰ����ȵ����ݣ��������ܵ���������Ӧ���ݰ��ṹ�洢
				recv(_cliSock, (char *)&login, sizeof(LogIn), 0);
				/*
					//���ڴ˴���չ��½ҵ���߼�
				*/
				printf("��������UserName = '%s', PassWord = '%s' \n", login.UserName, login.PassWord);
				//��Ӧҵ������ɺ���Է����Ͱ�ͷ
				if (SOCKET_ERROR == send(_cliSock, (const char*)&header, sizeof(DataHeader), 0))
				{
					//��ȡ������
					int eroCode = WSAGetLastError();
					printf("ERROR(������: %d)�����ݷ���ʧ�ܣ�\n", eroCode);
					//����׽���
					closesocket(_cliSock);
					break;
				}
				//���彫Ҫ���͵�log״̬
				LogResult logR = { 1 };		//1����ǰ����LOG_IN״̬
				send(_cliSock, (const char*)&logR, sizeof(LogResult), 0);
				break;
			}
			case ORDER_LOG_OUT:
			{
				LogOut logout = {};
				recv(_cliSock, (char *)&logout, sizeof(LogOut), 0);
				/*
					//���ڴ˴���չ��½ҵ���߼�
				*/
				printf("�ǳ�����UserName = '%s' \n", logout.UserName);
				if (SOCKET_ERROR == send(_cliSock, (const char*)&header, sizeof(DataHeader), 0))
				{
					//��ȡ������
					int eroCode = WSAGetLastError();
					printf("ERROR(������: %d)�����ݷ���ʧ�ܣ�\n", eroCode);
					//����׽���
					closesocket(_cliSock);
					break;
				}
				LogResult logR = { 0 };		//0����ǰ����LOG_OUT״̬
				send(_cliSock, (const char*)&logR, sizeof(LogResult), 0);
				break;
			}
			//���յ��쳣ָ��ʱ
			default:
			{
				header.ORDER = ORDER_ERROR;
				header.DataLength = 0;
				send(_cliSock, (const char*)&header, sizeof(DataHeader), 0);
			}
				break;
			}
		}
	}
	closesocket(_serSock);
	WSACleanup();
	system("pause");
	return 0;
}