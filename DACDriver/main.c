//#define MAIN
#ifdef MAIN
#include "Header.h"
#include <stdio.h>
int main(void)
{
#endif
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
#ifdef MAIN
}
#endif