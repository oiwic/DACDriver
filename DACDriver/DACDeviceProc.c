#include"Header.h"

UINT WINAPI DeviceProc(LPVOID lpParameter)
{
	DevicePara  *pPara   = (DevicePara*)lpParameter;
	TaskList	**ppFirst  = pPara->ppTaskList;
	UINT		*pFlag   = pPara->pExitFlag;
	HANDLE		*pHandle_Device = pPara->pHMutex_Device;
	HANDLE		*pHandle_Main	= pPara->pHMutex_Main;
	SOCKET		*pSockDevice = pPara->pSocket;
	free(lpParameter);
	
	while(1)
	{
		DWORD obj;
		TaskList *pFirstTask = *ppFirst;
		obj = WaitForSingleObject(*pHandle_Main,10);
		if(WAIT_OBJECT_0 == obj && *pFlag == 0)
		{
			while(pFirstTask != NULL)
			{
				pFirstTask->pFunc(pSockDevice,pFirstTask->ctrlCmd,&(pFirstTask->resp),pFirstTask->pData);
				pFirstTask = pFirstTask->pNext;
			}
			ReleaseSemaphore(*pHandle_Device,1,0);
		}
		else
		{
			if(*pFlag != 0)
			{
				ReleaseSemaphore(*pHandle_Device,1,0);
				break;
			}	
		}
	}
	return 0;
}