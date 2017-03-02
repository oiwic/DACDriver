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
}
#endif