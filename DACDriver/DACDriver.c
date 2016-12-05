#define DLLAPI  __declspec(dllexport)
#include "Header.h"

DACDeviceList* pFirst = NULL;		//First device pointer.
int g_IsParallel = SERIAL;			//Indicate whether softwre run as parallel mode.

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

void AddTask(DACDeviceList *pSelect,TaskList *pNew)
{
	if(pNew != NULL && pSelect != NULL)
	{
		pNew->pNext =  pSelect->pFirst;
		pSelect->pFirst = pNew;
	}
}

void DeleteAllTask(TaskList *pFirstTask)
{
	TaskList *pTemp;
	while(pFirstTask)
	{
		pTemp = pFirstTask->pNext;
		free(pFirstTask->pData);
		free(pFirstTask);
		pFirstTask = pTemp;
	}
}

DLLAPI int Open(UINT *id,char* ip,WORD port)
{	
	DACDeviceList *pNew;
	WORD wVersionRequest;
	DevicePara *pPara;
	int deviceID = inet_addr(ip);

	/*If device exsist, direct return. */
	pNew = FindList(deviceID);
	if(pNew != NULL) return RES_OK;

	pNew = (DACDeviceList*)malloc(sizeof(DACDeviceList)); //Add a device node.
	pPara = (DevicePara*)malloc(sizeof(DevicePara));	  //The parameter for device thread.

	pNew->socketInfo.addrSrv.sin_family = AF_INET;
	pNew->socketInfo.addrSrv.sin_port = htons(port);
	pNew->socketInfo.addrSrv.sin_addr.S_un.S_addr = deviceID;
	*id = deviceID;										 //Return the id of device to caller.

	pNew->exitFlag = 0;
	pNew->hMutex_Device = CreateSemaphore(0,1,1,0);			 //Signaled，The device thread release signal when device finished the tasks.
	pNew->hMutex_Main = CreateSemaphore(0,0,1,0);			 //Unsingnal, The main therad release signal when start the device thread.
	pNew->id = *id;
	pNew->pNext = NULL;
	pNew->pFirst = NULL;

	wVersionRequest = MAKEWORD(2,2);
	if(WSAStartup(wVersionRequest,&(pNew->socketInfo.wsaData)) != 0) //Failed to request for certain version of winsock.
	{
		free(pNew);
		free(pPara);
		return RES_ERR;
	}
	pNew->socketInfo.sockClient = socket(AF_INET,SOCK_STREAM,0);//Create a stream sock.
	if(INVALID_SOCKET == pNew->socketInfo.sockClient)
	{
		WSACleanup();
		free(pNew);
		free(pPara);
		return RES_ERR;
	}//创建套接字失败
	if(connect(pNew->socketInfo.sockClient,(SOCKADDR*)&(pNew->socketInfo.addrSrv),sizeof(pNew->socketInfo.addrSrv)) == RES_ERR)
	{
		WSACleanup();
		free(pNew);
		free(pPara);
		return RES_ERR;
	}	//连接失败

	/* Assign parameter for device thread. */
	pPara->pExitFlag  = &(pNew->exitFlag);
	pPara->pHMutex_Device = &(pNew->hMutex_Device);
	pPara->pHMutex_Main = &(pNew->hMutex_Main);
	pPara->ppTaskList = &(pNew->pFirst);
	pPara->pSocket	  = &(pNew->socketInfo.sockClient);
	
	AddList(pNew);//Add the new device to list.
	pNew->hThread = (HANDLE)_beginthreadex(0,0,DeviceProc,pPara,0,0);//Create device thread and run immediately.

	return RES_OK;
}

DLLAPI int Close(UINT id)
{
	DACDeviceList *pNow;
	pNow = FindList(id);
	if(pNow == NULL) return RES_ERR;
	pNow->exitFlag = 1;
	if(g_IsParallel == SERIAL && WAIT_OBJECT_0 == WaitForSingleObject(pNow->hThread,INFINITE))
	{
		CloseHandle(pNow->hThread);
		CloseHandle(pNow->hMutex_Main);
		CloseHandle(pNow->hMutex_Device);
		closesocket(pNow->socketInfo.sockClient);
		WSACleanup();
		DeleteAllTask(pNow->pFirst);
		DeleteList(pNow);
		return RES_OK;
	}
	return RES_ERR;
}

DLLAPI int WriteInstruction(UINT id,UINT instruction,UINT para1,UINT para2)
{
	DACDeviceList* pSelect = FindList(id);
	if(pSelect == NULL) return RES_ERR;
	if(g_IsParallel)
	{
		TaskList *pTaskList = (TaskList*)malloc(sizeof(TaskList));
		pTaskList->funcType = FixParameterSend;
		pTaskList->pFunc = &RWInstructionExe;
		pTaskList->ctrlCmd.instrction = instruction;
		pTaskList->ctrlCmd.para1 = para1;
		pTaskList->ctrlCmd.para2 = para2;
		pTaskList->pData = NULL;
		pTaskList->pNext = NULL;
		AddTask(pSelect,pTaskList);
		return RES_WAIT;
	}
	else
	{
		if( SERIAL== g_IsParallel && WAIT_OBJECT_0 == WaitForSingleObject(pSelect->hMutex_Device,INFINITE))//Must wait for device thread finish it's task.
		{
			CtrlCmd ctrlCmd;
			Resp resp;
			ctrlCmd.instrction = instruction;
			ctrlCmd.para1 = para1;
			ctrlCmd.para2 = para2;
			if(0 == RWInstructionExe(&(pSelect->socketInfo.sockClient),ctrlCmd,&resp,0))
			{
				ReleaseSemaphore(pSelect->hMutex_Device,1,0);
				if(resp.stat == STAT_SUCCESS)return RES_OK;
				else return RES_ERR;
			}
			ReleaseSemaphore(pSelect->hMutex_Device,1,0);
		}
		return RES_ERR;
	}
}

DLLAPI int ReadInstruction(UINT id,UINT instruction,UINT para1,UINT *para2)
{
	DACDeviceList* pSelect = FindList(id);
	if(pSelect == NULL) return RES_ERR;

	if(g_IsParallel)
	{
		TaskList *pTaskList = (TaskList*)malloc(sizeof(TaskList));
		pTaskList->funcType = FixParameterRecv;
		pTaskList->pFunc = &RWInstructionExe;
		pTaskList->ctrlCmd.instrction = instruction;
		pTaskList->ctrlCmd.para1 = para1;
		pTaskList->ctrlCmd.para2 = 0;
		pTaskList->pData = NULL;
		pTaskList->pNext = NULL;
		AddTask(pSelect,pTaskList);
		return RES_WAIT;
	}
	else
	{
		if( SERIAL== g_IsParallel && WAIT_OBJECT_0 == WaitForSingleObject(pSelect->hMutex_Device,INFINITE))//Must wait for device thread finish it's task.
		{
			CtrlCmd ctrlCmd;
			Resp resp;
			ctrlCmd.instrction = instruction;
			ctrlCmd.para1 = para1;
			ctrlCmd.para2 = 0;
			if(0 == RWInstructionExe(&(pSelect->socketInfo.sockClient),ctrlCmd,&resp,0))
			{
				ReleaseSemaphore(pSelect->hMutex_Device,1,0);
				if(resp.stat == STAT_SUCCESS)
				{
					*para2 = resp.data;
					return RES_OK;
				}
				else return RES_ERR;
			}
			ReleaseSemaphore(pSelect->hMutex_Device,1,0);
		}
		return RES_ERR;
	}
}

DLLAPI int WriteMemory(UINT id,UINT instruction,UINT start,UINT length,WORD* pData)
{
	DACDeviceList* pSelect = FindList(id);
	if(pSelect == NULL) return RES_ERR;

	if(g_IsParallel)
	{
		TaskList *pTaskList = (TaskList*)malloc(sizeof(TaskList));
		pTaskList->funcType = FixParameterRecv;
		pTaskList->pFunc = &WriteMemoryExe;
		pTaskList->ctrlCmd.instrction = instruction;
		pTaskList->ctrlCmd.para1 = start;
		pTaskList->ctrlCmd.para2 = length;
		pTaskList->pData = (char*)malloc(length);
		pTaskList->pNext = NULL;
		memcpy(pTaskList->pData,pData,length);
		AddTask(pSelect,pTaskList);
		return RES_WAIT;
	}
	else
	{
		if( SERIAL== g_IsParallel && WAIT_OBJECT_0 == WaitForSingleObject(pSelect->hMutex_Device,INFINITE))//Must wait for device thread finish it's task.
		{
			CtrlCmd ctrlCmd;
			Resp resp;
			ctrlCmd.instrction = instruction;
			ctrlCmd.para1 = start;
			ctrlCmd.para2 = length;
			if(0 == WriteMemoryExe(&(pSelect->socketInfo.sockClient),ctrlCmd,&resp,(char*)pData))
			{
				ReleaseSemaphore(pSelect->hMutex_Device,1,0);
				if(resp.stat == STAT_SUCCESS) return RES_OK;
				else return RES_ERR;
			}
			ReleaseSemaphore(pSelect->hMutex_Device,1,0);
		}
		return RES_ERR;
	}
}

DLLAPI int ReadMemory(UINT id,UINT instruction,UINT start,UINT length,WORD* pData)
{
	DACDeviceList* pSelect = FindList(id);
	if(pSelect == NULL) return RES_ERR;

	if(g_IsParallel == PARALLEL)//In this mode, device threads are idle.
	{
		TaskList *pTaskList = (TaskList*)malloc(sizeof(TaskList));
		pTaskList->funcType = FixParameterRecv;
		pTaskList->pFunc = &ReadMemoryExe;
		pTaskList->ctrlCmd.instrction = instruction;
		pTaskList->ctrlCmd.para1 = start;
		pTaskList->ctrlCmd.para2 = length;
		pTaskList->pData = (char*)malloc(length);
		pTaskList->pNext = NULL;
		AddTask(pSelect,pTaskList);
		return RES_WAIT;
	}
	else
	{
		if( SERIAL== g_IsParallel && WAIT_OBJECT_0 == WaitForSingleObject(pSelect->hMutex_Device,INFINITE))//Must wait for device thread finish it's task.
		{
			CtrlCmd ctrlCmd;
			Resp resp;
			ctrlCmd.instrction = instruction;
			ctrlCmd.para1 = start;
			ctrlCmd.para2 = length;
			if(0 == ReadMemoryExe(&(pSelect->socketInfo.sockClient),ctrlCmd,&resp,(char*)pData))
			{
				ReleaseSemaphore(pSelect->hMutex_Device,1,0);
				if(resp.stat == STAT_SUCCESS) return RES_OK;
				else return RES_ERR;
			}
			ReleaseSemaphore(pSelect->hMutex_Device,1,0);
		}
		return RES_ERR;
	}
}

DLLAPI int StartTask(UINT id)
{
	DACDeviceList* pTemp = pFirst;
	if(g_IsParallel == SERIAL) return RES_ERR;//Serial mode can not run StartTask.
	while(pTemp)							  //Lunch device thread. this function is called after SetTaskMode.
	{
		ReleaseSemaphore(pTemp->hMutex_Main,1,0);	  //Release signal for device.
		pTemp = pTemp->pNext;
	}
	g_IsParallel = SERIAL;					  //Auto reset run mode.
	return RES_OK;
}

DLLAPI int SetTimeOut(UINT id,float time)
{
	DACDeviceList* pSelect = FindList(id);
	int timeOut = (int)(time*1000);
	if(pSelect == NULL) return RES_ERR;
	if(WAIT_OBJECT_0 == WaitForSingleObject(pSelect->hMutex_Device,10))
	{
		if(setsockopt(pSelect->socketInfo.sockClient,SOL_SOCKET,SO_RCVTIMEO,(char*)&timeOut,sizeof(int)))return RES_ERR;
		if(setsockopt(pSelect->socketInfo.sockClient,SOL_SOCKET,SO_SNDTIMEO,(char*)&timeOut,sizeof(int)))return RES_ERR;
		ReleaseSemaphore(pSelect->hMutex_Device,1,0);
		return RES_OK;
	}
	return RES_ERR;
}

DLLAPI int SetTaskMode()
{
	if(g_IsParallel == PARALLEL) return RES_OK;	//Do not need to set again
	else										//Change mode from serial to parallel.
	{
		DACDeviceList* pTemp = pFirst;
		while(pTemp != NULL)
		{
			DWORD obj = WaitForSingleObject(pTemp->hMutex_Device,INFINITE);
			if(obj == WAIT_OBJECT_0) 	DeleteAllTask(pTemp->pFirst);
			else return RES_ERR;	
			pTemp = pTemp->pNext;
		}
		g_IsParallel = PARALLEL;
		return RES_OK;
	}
}

DLLAPI int GetReturn(UINT id,WORD *pData,UINT isStart)
{
	DACDeviceList* pSelect = FindList(id);
	static TaskList* pNow = NULL;
	if(pSelect == NULL)	return RES_ERR;
	if(pNow == NULL || isStart == 1) pNow = pSelect->pFirst;
	while(pNow != NULL)
	{
		if(pNow->funcType == FlexParameterRecv)
		{
			memcpy(pData,pNow->pData,pNow->ctrlCmd.para2);
			pNow = pNow->pNext;
			return 0;
		}
		else if(pNow->funcType == FixParameterRecv)
		{
			memcpy(pData,&(pNow->resp.data),sizeof(pNow->resp.data));
			pNow = pNow->pNext;
			return 0;
		}
		else
			pNow = pNow->pNext;
	}
	return RES_ERR;
}

DLLAPI int CheckFinished(UINT *isFinished)
{
	DACDeviceList* pTemp = pFirst;
	UINT obj;
	while(pTemp != NULL)
	{
		obj = WaitForSingleObject(pTemp->hMutex_Device,10);
		if(obj != WAIT_OBJECT_0){*isFinished = 0; return RES_OK;}
		ReleaseSemaphore(pTemp->hMutex_Device,1,0);					//Release Mutex to avoid influencing the status. 
		pTemp = pTemp->pNext;
	}
	*isFinished = 1;
	return RES_OK;
}