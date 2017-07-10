/*
	FileName:DACDeviceProc.c
	Author:GuoCheng
	E-mail:fortune@mail.ustc.edu.cn
	All right reserved @ GuoCheng.
	Modified: 2017.2.15
	Description: The realiztion of device thread.
*/

#include"Header.h"

UINT WINAPI DeviceProc(LPVOID lpParameter)
{
	DevicePara  *pPara   = (DevicePara*)lpParameter;
	TaskList	*pFirst  = pPara->pTask;
	UINT		*pFlag   = pPara->pExitFlag;
	HANDLE		*pSemaphoreSpace = pPara->pSemaphoreSpace;
	HANDLE		*pSemaphoreTask   = pPara->pSemaphoreTask;
	SOCKET		*pSockDevice = pPara->pSocket;
	UINT		*pDeviceCounter = pPara->pDeviceCounter;
	free(lpParameter);
	
	while(1)
	{
		DWORD obj;
		obj = WaitForSingleObject(*pSemaphoreTask,10);
		if(*pFlag == 0)
		{
			if(WAIT_OBJECT_0 == obj)
			{
				(pFirst+(*pDeviceCounter))->pFunc(pSockDevice,(pFirst+(*pDeviceCounter))->ctrlCmd,&((pFirst + (*pDeviceCounter))->resp),(pFirst + (*pDeviceCounter))->pData);
				*pDeviceCounter = ((*pDeviceCounter)+1)%WAIT_TASK_MAX;
				ReleaseSemaphore(*pSemaphoreSpace,1,0);
			}
			else
				continue;
		}
		else
		{
			if(WAIT_OBJECT_0 == obj)
			{
				*pDeviceCounter = ((*pDeviceCounter)+1)%WAIT_TASK_MAX;
				ReleaseSemaphore(*pSemaphoreSpace,1,0);
			}
			else
				break;
		}
	}
	return 0;
}