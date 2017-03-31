/*
	FileName:DACDriver.c
	Author:GuoCheng
	E-mail:fortune@mail.ustc.edu.cn
	All right reserved @ GuoCheng.
	Modified: 2017.2.25
	Description: The main body of DACDriver.
*/

#define DLLAPI  __declspec(dllexport)
#include "Header.h"

DACDeviceList* pFirst = NULL;		//First device pointer.

void AddList(DACDeviceList*pNow)
{
	if(pNow != NULL)
	{
		pNow->pNext = pFirst;
		pFirst = pNow;
	}
}

void DeleteList(DACDeviceList*pNow)
{
	DACDeviceList *pTemp = pFirst;
	if(pNow == NULL || pFirst == NULL)
		return;
	if(pNow == pFirst)
	{
		pFirst = pTemp->pNext;
		free(pTemp);
	}
	else
	{
		while(pTemp->pNext != pNow && pTemp->pNext != NULL)
		{
			pTemp = pTemp->pNext;
		}
		if(pTemp->pNext == pNow)
		{
			pTemp->pNext = pNow->pNext;
			free(pNow);
		}
	}
}

DACDeviceList* FindList(UINT id)
{
	DACDeviceList *pTemp = pFirst;
	while(pTemp != NULL)
	{
		if(pTemp->id == id)
			return pTemp;
		else
			pTemp = pTemp->pNext;
	}
	return NULL;
}

void InitTask(TaskList *pTask,int num)
{
	int i = 0;
	for(i=0;i<num;i++)
	{
		memset(pTask+i,0,sizeof(TaskList));
	}
}

void ClearTask(TaskList* pTask,int num)
{
	int i = 0;
	for(i=0;i<num;i++)
	{
		free((pTask+i)->pData);
	}
}

DLLAPI int Open(UINT *pID,char* ip,WORD port)
{	
	DACDeviceList *pNew;
	WORD wVersionRequest;
	DevicePara *pPara;
	UINT deviceID = inet_addr(ip);

	/* If device exist, direct return. */
	pNew = FindList(deviceID);
	if(pNew != NULL) 
	{
		*pID = deviceID; //retrun ID
		return RES_OK;
	}

	/* If device does not exist, generate a now device */
	pNew  = (DACDeviceList*)malloc(sizeof(DACDeviceList));

	pNew->socketInfo.addrSrv.sin_family = AF_INET;
	pNew->socketInfo.addrSrv.sin_port = htons(port);
	pNew->socketInfo.addrSrv.sin_addr.S_un.S_addr = deviceID;
	wVersionRequest = MAKEWORD(2,2);

	if(WSAStartup(wVersionRequest,&(pNew->socketInfo.wsaData)) != 0) //Failed to request for certain version of winsock.
	{
		free(pNew);
		return RES_ERR;
	}
	pNew->socketInfo.sockClient = socket(AF_INET,SOCK_STREAM,0);//Create a stream sock.
	if(INVALID_SOCKET == pNew->socketInfo.sockClient)
	{
		WSACleanup();
		free(pNew);
		return RES_ERR;
	}//创建套接字失败
	if(connect(pNew->socketInfo.sockClient,(SOCKADDR*)&(pNew->socketInfo.addrSrv),sizeof(pNew->socketInfo.addrSrv)) != 0)
	{
		WSACleanup();
		free(pNew);
		return RES_ERR;
	}//连接失败

	
	pNew->semaphoreSpace = CreateSemaphore(0,WAIT_TASK_MAX,WAIT_TASK_MAX,0);		//Signaled，The device thread release signal when device finished the tasks.
	pNew->semaphoreTask  = CreateSemaphore(0,0,WAIT_TASK_MAX,0);					//Unsingnal, The main therad release signal when start the device thread.
	pNew->exitFlag = 0;
	pNew->id = deviceID;
	pNew->pNext = NULL;
	pNew->mainCounter = 0;
	pNew->deviceCounter = 0;
	InitTask(&(pNew->task[0]),WAIT_TASK_MAX);

	/* Assign parameter for device thread. */
	pPara = (DevicePara*)malloc(sizeof(DevicePara));
	pPara->pExitFlag  = &(pNew->exitFlag);
	pPara->pSemaphoreSpace = &(pNew->semaphoreSpace);
	pPara->pSemaphoreTask  = &(pNew->semaphoreTask);
	pPara->pTask = &(pNew->task[0]);
	pPara->pSocket	= &(pNew->socketInfo.sockClient);
	pPara->pDeviceCounter = &(pNew->deviceCounter);
	
	pNew->hThread = (HANDLE)_beginthreadex(0,0,DeviceProc,pPara,0,0);//Create device thread and run immediately.

	AddList(pNew);   //Add the new device to list.
	*pID = deviceID; //retrun ID

	return RES_OK;
}

DLLAPI int Close(UINT id)
{
	DACDeviceList *pNow;
	pNow = FindList(id);
	if(pNow == NULL) return RES_ERR;
	pNow->exitFlag = 1;
	if(WAIT_OBJECT_0 == WaitForSingleObject(pNow->hThread,INFINITE))
	{
		CloseHandle(pNow->hThread);
		CloseHandle(pNow->semaphoreTask);
		CloseHandle(pNow->semaphoreSpace);
		closesocket(pNow->socketInfo.sockClient);
		WSACleanup();
		ClearTask(&(pNow->task[0]),WAIT_TASK_MAX);
		DeleteList(pNow);
		return RES_OK;
	}
	return RES_ERR;
}

DLLAPI int WriteInstruction(UINT id,UINT instruction,UINT para1,UINT para2)
{
	DACDeviceList* pSelect = FindList(id);
	DWORD obj;
	if(pSelect == NULL) return RES_ERR;
	obj = WaitForSingleObject(pSelect->semaphoreSpace,10);
	if(obj == WAIT_OBJECT_0)
	{
		pSelect->task[pSelect->mainCounter].ctrlCmd.instrction = instruction;
		pSelect->task[pSelect->mainCounter].ctrlCmd.para1 = para1;
		pSelect->task[pSelect->mainCounter].ctrlCmd.para2 = para2;
		pSelect->task[pSelect->mainCounter].resp.stat = -1;
		pSelect->task[pSelect->mainCounter].funcType = FixParameterSend;
		pSelect->task[pSelect->mainCounter].pData = NULL;
		pSelect->task[pSelect->mainCounter].pFunc = &RWInstructionExe;
		pSelect->mainCounter = ((pSelect->mainCounter) + 1)%WAIT_TASK_MAX;
		ReleaseSemaphore(pSelect->semaphoreTask,1,0);
		return RES_OK;
	}
	return RES_ERR;
}

DLLAPI int ReadInstruction(UINT id,UINT instruction,UINT para1)
{
	DACDeviceList* pSelect = FindList(id);
	DWORD obj;
	if(pSelect == NULL) return RES_ERR;
	obj = WaitForSingleObject(pSelect->semaphoreSpace,10);
	if(obj == WAIT_OBJECT_0)
	{
		pSelect->task[pSelect->mainCounter].ctrlCmd.instrction = instruction;
		pSelect->task[pSelect->mainCounter].ctrlCmd.para1 = para1;
		pSelect->task[pSelect->mainCounter].ctrlCmd.para2 = 0;
		pSelect->task[pSelect->mainCounter].resp.stat = -1;
		pSelect->task[pSelect->mainCounter].funcType = FixParameterRecv;
		pSelect->task[pSelect->mainCounter].pData = NULL;
		pSelect->task[pSelect->mainCounter].pFunc = &RWInstructionExe;
		pSelect->mainCounter = ((pSelect->mainCounter)++)%WAIT_TASK_MAX;
		ReleaseSemaphore(pSelect->semaphoreTask,1,0);
		return RES_OK;
	}
	return RES_ERR;

}

DLLAPI int WriteMemory(UINT id,UINT instruction,UINT start,UINT length,WORD* pData)
{

	DACDeviceList* pSelect = FindList(id);
	DWORD obj;
	if(pSelect == NULL) return RES_ERR;
	obj = WaitForSingleObject(pSelect->semaphoreSpace,10);
	if(obj == WAIT_OBJECT_0)
	{
		pSelect->task[pSelect->mainCounter].ctrlCmd.instrction = instruction;
		pSelect->task[pSelect->mainCounter].ctrlCmd.para1 = start;
		pSelect->task[pSelect->mainCounter].ctrlCmd.para2 = length;
		pSelect->task[pSelect->mainCounter].resp.stat = -1;
		pSelect->task[pSelect->mainCounter].funcType = FixParameterSend;
		free(pSelect->task[pSelect->mainCounter].pData);
		pSelect->task[pSelect->mainCounter].pData = (char*)malloc(length);
		memcpy(pSelect->task[pSelect->mainCounter].pData,pData,length);
		pSelect->task[pSelect->mainCounter].pFunc = &WriteMemoryExe;
		pSelect->mainCounter = ((pSelect->mainCounter) + 1)%WAIT_TASK_MAX;
		ReleaseSemaphore(pSelect->semaphoreTask,1,0);
		return RES_OK;
	}
	return RES_ERR;
}

DLLAPI int ReadMemory(UINT id,UINT instruction,UINT start,UINT length)
{
	DACDeviceList* pSelect = FindList(id);
	DWORD obj;
	if(pSelect == NULL) return RES_ERR;
	obj = WaitForSingleObject(pSelect->semaphoreSpace,10);
	if(obj == WAIT_OBJECT_0)
	{
		pSelect->task[pSelect->mainCounter].ctrlCmd.instrction = instruction;
		pSelect->task[pSelect->mainCounter].ctrlCmd.para1 = start;
		pSelect->task[pSelect->mainCounter].ctrlCmd.para2 = length;
		pSelect->task[pSelect->mainCounter].resp.stat = -1;
		pSelect->task[pSelect->mainCounter].funcType = FixParameterRecv;
		free(pSelect->task[pSelect->mainCounter].pData);
		pSelect->task[pSelect->mainCounter].pData = (char*)malloc(length);
		pSelect->task[pSelect->mainCounter].pFunc = &ReadMemoryExe;
		pSelect->mainCounter = ((pSelect->mainCounter)++)%WAIT_TASK_MAX;
		ReleaseSemaphore(pSelect->semaphoreTask,1,0);
		return RES_OK;
	}
	return RES_ERR;
}

DLLAPI int SetTimeOut(UINT id,UINT direction,float time)
{
	DACDeviceList* pSelect = FindList(id);
	UINT timeOut = (UINT)(time*1000);
	if(pSelect == NULL) return RES_ERR;
	WaitUntilFinished(id);
	if(direction == 0)
	{
		if(0 == setsockopt(pSelect->socketInfo.sockClient,SOL_SOCKET,SO_RCVTIMEO,(char*)&timeOut,sizeof(int)))
			return RES_OK;
		else 
			return RES_ERR;
	}
	else
	{
		
		if(0 == setsockopt(pSelect->socketInfo.sockClient,SOL_SOCKET,SO_SNDTIMEO,(char*)&timeOut,sizeof(int)))
			return RES_OK;
		else
			return RES_ERR;
	}
}

DLLAPI int GetFunctionType(UINT id,UINT offset,UINT *pFunctype,UINT *pInstruction,UINT *pPara1,UINT *pPara2)
{
	DACDeviceList* pSelect = FindList(id);
	if(pSelect == NULL)	return RES_ERR;
	if(offset >= WAIT_TASK_MAX) return RES_ERR;
	WaitUntilFinished(id);
	offset = (pSelect->mainCounter + WAIT_TASK_MAX - offset)%WAIT_TASK_MAX;
	if(pSelect->task[offset].pFunc == NULL) return RES_ERR;
	if(pSelect->task[offset].resp.stat != STAT_SUCCESS) return RES_ERR;
	*pInstruction = pSelect->task[offset].ctrlCmd.instrction;
	*pPara1 = pSelect->task[offset].ctrlCmd.para1;
	*pPara2 = pSelect->task[offset].ctrlCmd.para2;
	switch(pSelect->task[offset].funcType)
	{
	case FixParameterSend: *pFunctype = 1; break;
	case FixParameterRecv: *pFunctype = 2; break;
	case FlexParameterSend:*pFunctype = 3; break;
	case FlexParameterRecv:*pFunctype = 4; break;
	}
	return RES_OK;
}

DLLAPI int GetReturn(UINT id,UINT offset,WORD *pData)
{
	DACDeviceList* pSelect = FindList(id);
	if(pSelect == NULL)	return RES_ERR;
	if(offset >= WAIT_TASK_MAX) return RES_ERR;
	WaitUntilFinished(id);
	offset = (pSelect->mainCounter + WAIT_TASK_MAX - offset)%WAIT_TASK_MAX;
	if(pSelect->task[offset].pFunc == NULL) return RES_ERR;
	if(pSelect->task[offset].resp.stat != STAT_SUCCESS) return RES_ERR;
	switch(pSelect->task[offset].funcType)
	{
	case FixParameterSend: memcpy(pData,&(pSelect->task[offset].resp.data),4);break;
	case FixParameterRecv: memcpy(pData,&(pSelect->task[offset].resp.data),4);break;
	case FlexParameterSend:memcpy(pData,pSelect->task[offset].pData,pSelect->task[offset].ctrlCmd.para2);break;
	case FlexParameterRecv:memcpy(pData,pSelect->task[offset].pData,pSelect->task[offset].ctrlCmd.para2);break;
	}
	return RES_OK;
}

DLLAPI int CheckFinished(UINT id,UINT *isFinished)
{
	DACDeviceList* pSelect = FindList(id);
	if(pSelect != NULL)
	{
		if(pSelect->mainCounter == pSelect->deviceCounter)
			*isFinished = 1;
		else
			*isFinished = 0;
		return RES_OK;
	}
	*isFinished = 0;
	return RES_ERR;
}

DLLAPI int WaitUntilFinished(UINT id)
{
	UINT isFinished = 0;
	int ret = RES_OK;
	while(!isFinished && ret == RES_OK)
	{
		ret = CheckFinished(id,&isFinished);
		Sleep(1);
	}
	return ret;
}

DLLAPI int GetSoftInformation(char *pInformation)
{
	memcpy(pInformation,DAC_DESCRIPTION,strlen(DAC_DESCRIPTION));
	pInformation[strlen(DAC_DESCRIPTION)] = 0;
	return RES_OK;
}

DLLAPI int ScanDevice(char *pDeviceList)
{
	return RES_OK;
}

DLLAPI int CheckSuccessed(UINT id,UINT *pIsSuccessed)
{
	DACDeviceList* pSelect = FindList(id);
	UINT i = 0;
	if(pSelect == NULL)	return RES_ERR;
	WaitUntilFinished(id);
	*pIsSuccessed = 1;
	while(i < WAIT_TASK_MAX && pSelect->task[i].pFunc != NULL)
	{
		if(pSelect->task[i].resp.stat != STAT_SUCCESS)
		{
			*pIsSuccessed = 0;
			break;
		}
		i++;
	}
	return RES_OK;
}