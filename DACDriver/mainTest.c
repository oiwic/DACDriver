/*
	FileName:mainTest.c
	Author:GuoCheng
	E-mail:fortune@mail.ustc.edu.cn
	All right reserved @ GuoCheng.
	Modified: 2017.4.3
	Description: This file is used to test function.
*/

//#define MAIN
#ifdef MAIN
#include "Header.h"
#include <stdio.h>
int main(void)
{
	/* function test 20170224
	char ip[] = "10.0.1.219";
	int ret;
	unsigned char seq[32768];
	unsigned short data[32];
	unsigned int id;
	int i = 0;
	for(i=0;i<32;i++)
	{
		data[i] = 256*i;
	}
	for(i=0;i<4096;i++)
	{
		seq[4*i] = 0;
	}
	
	Open(&id,ip,80);

	for(i=0;i<2000;i++)
		WriteMemory(id,4,0,64,&data[0]);
	Close(id);
	*/

	/*function test
	char ip1[] = "172.16.60.179";
	char ip2[] = "172.16.60.234";
	unsigned int id1;
	unsigned int id2;
	int ret;
	ret = Open(&id1,ip1,80);
	printf("ret = %d",ret);
	ret = Open(&id2,ip2,80);
	printf("ret = %d",ret);
	ret = Close(id1);
	printf("ret = %d",ret);
	ret = Close(id2);
	printf("ret = %d",ret);
	*/

	/*	
	HANDLE hMutex;
	long count;
	hMutex = CreateSemaphore(0,1,2,0);
	WaitForSingleObject(hMutex,10);
	ReleaseSemaphore(hMutex,1,(LPLONG)&count);
	printf("%d",count);
	ReleaseSemaphore(hMutex,1,(LPLONG)&count);
	printf("%d",count);
	ReleaseSemaphore(hMutex,1,(LPLONG)&count);
	printf("%d",count);
	WaitForSingleObject(hMutex,10000);
	return 0;
	*/
	/* 测试输出延时
	char ip[] = "10.0.2.2";
	unsigned int id1;
	int ret;
	char data[12] = {0};
	unsigned int timeOut  =10000;
	WORD wVersionRequest;
	SocketInfo socketInfo;

	socketInfo.addrSrv.sin_family = AF_INET;
	socketInfo.addrSrv.sin_port = htons(80);
	socketInfo.addrSrv.sin_addr.S_un.S_addr = inet_addr(ip);
	wVersionRequest = MAKEWORD(2,2);

	if(WSAStartup(wVersionRequest,&(socketInfo.wsaData)) != 0) //Failed to request for certain version of winsock.
	{
		return WSAGetLastError();
	}
	socketInfo.sockClient = socket(AF_INET,SOCK_STREAM,0);//Create a stream sock.
	if(INVALID_SOCKET == socketInfo.sockClient)
	{
		WSACleanup();
		return WSAGetLastError();
	}//创建套接字失败
	if(connect(socketInfo.sockClient,(SOCKADDR*)&(socketInfo.addrSrv),sizeof(socketInfo.addrSrv)) != 0)
	{
		WSACleanup();
		return WSAGetLastError();
	}//连接失败
	setsockopt(socketInfo.sockClient,SOL_SOCKET,SO_RCVTIMEO,(char*)&timeOut,sizeof(int));
	recv(socketInfo.sockClient,(char*)data,8,0);
	return 0;
	*/
}
#endif

//#define MAIN1
#ifdef MAIN1
#include "Header.h"
#include <stdio.h>
int main(void)
{
	char data[1024];
	GetErrorMsg(ERR_NOFUNC,data);
	printf("%s",data);
}
#endif

//#define MAIN2
#ifdef MAIN2
#include "Header.h"
#include <stdio.h>
int main(void)
{
	int ret=0;
	unsigned int id;
	ret = Open(&id,"10.0.0.3",80);
	printf("%d",ret);
}
#endif

//#define MAIN3
#ifdef MAIN3
#include "Header.h"
#include <stdio.h>
int main(void)
{
	int ret=0;
	unsigned int id;
	ret = Open(&id,"10.0.1.108",80);
	printf("%d",ret);
	ret = Close(id);
	printf("%d",ret);
	return 0;
}
#endif