#include <winsock2.h>
#include<stdio.h>
#include<iostream>
#include<string>
#include <cstring>
#include <fstream>
using namespace std;
#pragma comment(lib,"ws2_32.lib")//��������� 

//TCP�������� 

void RecvFile(SOCKET sClient, string SaveFileName);
int connect();

int main()
{
	int temp = 10;
	while (true)
	{
		if (temp == 10)
		{
			temp = connect();
		}
		else
		{
		}
	}
	return 0;
}


int connect()
{
	//����winsock 
	WORD sockVersion = MAKEWORD(2, 2);//����ʹ�õ�winsock�汾 
	WSADATA wsaData;   // ʵ�ʷ��ص�winsock�汾  
	if (WSAStartup(sockVersion, &wsaData) != 0)
	{
	}
	//����socket 
	SOCKET slisten = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);//�����ֱ�ΪЭ���壬���ͣ�Э��� AF_INET����TCP/IP 
	if (slisten == INVALID_SOCKET)//�쳣���� 
	{
		printf("scoket error!\n");
		return 10;
	}
	//bind
	sockaddr_in sin; //�������˵��ַ 
	sin.sin_family = AF_INET; //Э���� 
	sin.sin_port = htons(8888); //�˿ںţ� htons�����������ֽ�˳���Ϊ�����ֽ�˳��16λ�� 
	sin.sin_addr.S_un.S_addr = INADDR_ANY;//������bindʱ��Ҫʹ�õ�ַͨ��
	if (bind(slisten, (LPSOCKADDR)&sin, sizeof(sin)) == SOCKET_ERROR)//LPSOCKADDR������ǿ��ת�� 
	{
		printf("bind error !\n");
		return 10;
	}
	//listen
	if (listen(slisten, 5) == SOCKET_ERROR)//5Ϊqueuesize,��������С 
	{ 
		printf("listen error !\n");
		return 10;
	}
	//����ʹ�õ���TCP ��socket stream,Ҫѭ����������
	SOCKET sClient;//��������
	sockaddr_in remoteAddr;
	int nAddrlen = sizeof(remoteAddr);
	char revData[1024];//buffer
	printf("waiting for connect...\n\n");
	while (true)
	{
		printf("command��");
		string data;
		getline(cin, data);//���̶�������
		const char* sendData;
		sendData = data.c_str(); //string��Ϊconst char*
		sClient = accept(slisten, (SOCKADDR *)&remoteAddr, &nAddrlen);//accept���½�һ��socket 
		if (sClient == INVALID_SOCKET)
		{
			printf("accept error !");
			continue;//���¿�ʼѭ�� 
		}
		printf("someone ip: %s\r\n", inet_ntoa(remoteAddr.sin_addr)); //inet��ip��ַ�ṹת���ַ��� �� \r�ǻس� 
		//��������		
		send(sClient, sendData, strlen(sendData), 0);
		//�������� 

		//�����ļ�
		if (data.find("pass") != string::npos)//pass-D:\\001.txt-D:\\002.txt
		{
			string savepath = data.substr(data.find_last_of("-") + 1);
			RecvFile(sClient, savepath);
		}
		//�����ַ���
		else
		{
			int ret = recv(sClient, revData, 1024, 0);
			printf("feedback��\n");
			while (true)
			{
				if (ret > 0)
				{
					revData[ret] = 0x00;
					printf(revData);
					ret = recv(sClient, revData, 1024, 0);
				}
				break;
			}
			printf("\n\n");
		}
		closesocket(sClient);
	}
	closesocket(slisten);
	WSACleanup();
}


void RecvFile(SOCKET sClient, string SaveFileName)
{
	cout << "receive start" << endl;
	const int bufferSize = 1024;
	char buffer[bufferSize] = { 0 };
	int readLen = 0;
	//string SaveFileName ; //���Ƿ�����Ҫ���յ��ļ��ı���·��
	ofstream desFile;
	desFile.open(SaveFileName.c_str(), ios::binary);
	if (!desFile)
	{
		return;
	}
	do
	{
		readLen = recv(sClient, buffer, bufferSize, 0);
		if (readLen == 0)
		{
			break;
		}
		else
		{
			desFile.write(buffer, readLen);
			cout << "receiving" <<  endl;
		}
	} while (true);
	cout << "receive over\n\n" << endl;
	desFile.close();
}