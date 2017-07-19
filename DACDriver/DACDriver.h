/*
	FileName:DACDriver.h
	Author:GuoCheng
	E-mail:fortune@mail.ustc.edu.cn
	All right reserved @ GuoCheng.
	Modified: 2017.6.20
	Description:
*/

#pragma once

/* Define software version */
#define DAC_DESCRIPTION ("USTCDAC DLL driver v1.7 @ 2017/7/6")

/* Define the channel of a DAC */
#define CHANNEL_NUM 4

/* Define the sample point of a channel. */
#define CHANNEL_POT 32768

/* Define the sequency number of a channel. */
#define CHANNEL_SEQ 4096

/* Define max wait task num */
#define WAIT_TASK_MAX 256

/* Define maximum message length */
#define MAX_MSGLENTH 1024

//Define four different functions.
typedef enum FunctionType
{
	WriteInstructionType,
	WriteMemoryType,
	ReadMemoryType
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
    int  stat;
    int  data;
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
	HANDLE	 semaphoreSpace;		//Mutex signal release by main thread.
	HANDLE   semaphoreTask;			//Mutex signal releaseed by Device thread.
	HANDLE hThread;					//Handle of thread.
	SocketInfo socketInfo;			//Store the information of the socket.
	TaskList task[WAIT_TASK_MAX];	//The first pointer of the task.
	UINT mainCounter;				//Indicate the position of free space, only access by main thread.
	UINT deviceCounter;				//Indicate the position of task, write by device thread and read by main thread.
	UINT taskCounter;				//Indicate the number of task the check function will consider.
	struct DACDeviceList *pNext;	//The Next Device pointer.
}DACDeviceList;

/* Parameter for device thread. */
typedef struct DevicePara
{
	TaskList *pTask;
	HANDLE	 *pSemaphoreSpace;
	HANDLE   *pSemaphoreTask;
	UINT	 *pExitFlag;
	SOCKET	 *pSocket;
	UINT	 *pDeviceCounter;
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