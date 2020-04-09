//����windows�ض����жϵ�ǰ�������л�������������Ӧͷ�ļ�
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
	����TCP�����
	����ģ�ͣ�C--S
	��Ϣ���ͣ����ݱ��� ��ͷ + ���ݰ� ��
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

int main(void)
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
	//�ȴ��ͻ�������
	sockaddr_in _cli;
	int _cliSize = sizeof(_cli);
	#ifdef _WIN32
	SOCKET _cliSock = accept(_serSock, (sockaddr*)&_cli, &_cliSize);
	#else
	SOCKET _cliSock = accept(_serSock, (sockaddr*)&_cli, (socklen_t*)&_cliSize);
	#endif
	if (INVALID_SOCKET == _cliSock)
	{
		#ifdef _WIN32
		//��ȡ������
		int eroCode = WSAGetLastError();
		printf("ERROR(������: %d)���ͻ�������ʧ�ܣ�\n", eroCode);
		//����׽���
		closesocket(_serSock);
		//���������
		WSACleanup();
		#else
		printf("ERROR(������: %d)���ͻ�������ʧ�ܣ�\n", -1);
		#endif
		return 0;
	}
	else
	{
		printf("�¿ͻ��������ӣ�SOCKET = %d, IP = %s\n", (int)_cliSock, inet_ntoa(_cli.sin_addr));
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
			#ifdef _WIN32
			//��ȡ������
			int eroCode = WSAGetLastError();
			printf("ERROR(������: %d)�����ݽ���ʧ�ܣ�\n", eroCode);
			//����׽���
			closesocket(_cliSock);
			#else
			printf("ERROR(������: %d)�����ݽ���ʧ�ܣ�\n", -1);
			#endif
			break;
		}
		else if (rLen <= 0)
		{
			printf("�ͻ������˳������������\n");
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
				LogIn login;
				//������Ӧ���ݰ����ȵ����ݣ��������ܵ���������Ӧ���ݰ��ṹ�洢
				recv(_cliSock, (char *)&login + sizeof(DataHeader), sizeof(LogIn) - sizeof(DataHeader), 0);
				/*
					//���ڴ˴���չ��½ҵ���߼�
				*/
				printf("��������DataLength = %d, UserName = '%s', PassWord = '%s' \n", login.DataLength, login.UserName, login.PassWord);
				//���彫Ҫ���͵�log״̬
				LogResult logR;		
				send(_cliSock, (const char*)&logR, sizeof(LogResult), 0);
				break;
			}
			case ORDER_LOG_OUT:
			{
				LogOut logout;
				//��Ϊ�ٴ�֮ǰ�Ѿ�������DataHeader���ȵ����ݣ����ڴ˽��е�ַƫ�ƣ��Լ����ճ��ȸı�
				recv(_cliSock, (char *)&logout + sizeof(DataHeader), sizeof(LogOut) - sizeof(DataHeader), 0);
				/*
					//���ڴ˴���չ��½ҵ���߼�
				*/
				printf("�ǳ�����DataLength = %d, UserName = '%s' \n", logout.DataLength, logout.UserName);
				LogResult logR;		//0����ǰ����LOG_OUT״̬
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
	#ifdef _WIN32
	closesocket(_serSock);
	WSACleanup();
	system("pause");
	#else
	close(_cliSock);
	close(_serSock);
	#endif
	return 0;
}