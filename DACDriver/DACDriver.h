#ifndef DACDRIVER_H
#define DACDRIVER_H

//Define four different functions.
typedef enum FunctionType
{
	FixParameterSend,
	FlexParameterSend,
	FixParameterRecv,
	FlexParameterRecv
}FunctionType;

/* Command struct. */
typedef struct CtrlCmd
{
	UINT instrction;
	UINT para1;
	UINT para2;
}CtrlCmd;

/* FPGA response data struct. */
typedef struct Resp
{
    UINT  stat;
    UINT  data;
}Resp;

/* Define function type, four function have same type. */
typedef int (*CommunicationFunc)(SOCKET*,CtrlCmd,Resp *pResp,char*);

/* TaskList managed by main thread and acessed by Device thread. */
typedef struct TaskList
{
	FunctionType funcType;		//Function type, decide which function will be called among the three functions.
	CommunicationFunc pFunc;	//functino pointer.
	CtrlCmd ctrlCmd;			//instructin to send to fpga.
	Resp   resp;				//response of the fpga.
	char*  pData;				//data for write memory or read memory.
	struct TaskList* pNext;		//The next tasklist pointer.
}TaskList;

/* Define a socketinfo struct. */
typedef struct SocketInfo
{
	SOCKET sockClient;
	SOCKADDR_IN addrSrv;
	WSADATA wsaData;
}SocketInfo;
/* Define DACDeviceList for main thread. */
typedef struct DACDeviceList
{
	UINT id;						//Identiry the device.
	UINT exitFlag;					//Exit Flag, the thread will exit if set this flag.
	HANDLE hMutex_Device;			//Mutex signal release by main thread.
	HANDLE hMutex_Main;				//Mutex signal releaseed by Device thread.
	HANDLE hThread;					//Handle of thread.
	SocketInfo socketInfo;			//Store the information of the socket.
	TaskList *pFirst;				//The first pointer of the task.
	struct DACDeviceList *pNext;	//The Next Device pointer.
}DACDeviceList;
/* Parameter for device thread. */
typedef struct DevicePara
{
	TaskList **ppTaskList;
	HANDLE	 *pHMutex_Device;
	HANDLE   *pHMutex_Main;
	UINT	 *pExitFlag;
	SOCKET	 *pSocket;
}DevicePara;
/* Add a device to list head */
void AddList(DACDeviceList *pNow);
/* Delete a device from list */
void DeleteList(DACDeviceList*pNow);
/* Add a task to task list. */
void AddTask(DACDeviceList*,TaskList *pNew);
/* Delete whole task list. */
void DeleteAllTask(TaskList *pFirst);
/* Find a device pointer by it's id */
DACDeviceList* FindList(UINT id);

#endif
