/*
	FileName:DACDriver.h
	Author:GuoCheng
	E-mail:fortune@mail.ustc.edu.cn
	All right reserved @ GuoCheng.
	Create at 2016.11.17
*/

#ifndef EXPORT_H
#define EXPORT_H

#ifdef DLLAPI
#else
#define DLLAPI __declspec(dllimport)
#endif

#ifndef WORD
#define WORD unsigned short
#endif

#ifndef UINT
#define UINT unsigned int
#endif

#ifndef LPVOID
#define LPVOID void*
#endif

#ifndef NULL
#define NULL 0
#endif

/* Define three return state. */
#define RES_OK   (0)
#define RES_WAIT (1)
#define RES_ERR (-1)

/* Open a device and add it to device list. */
DLLAPI int Open(UINT */*id*/,char*/*ip*/,WORD/*port*/);
/* Close the device and clear the data */
DLLAPI int Close(UINT/*id*/);
/* Write a command to FPGA */
DLLAPI int WriteInstruction(UINT/*id*/,UINT/*instructino*/,UINT/*para1*/,UINT/*para2*/);
/* Read a register from FPGA. */
DLLAPI int ReadInstruction(UINT/*id*/,UINT/*instruction*/,UINT/*addr*/,UINT*/*pReg*/);
/* Write data to DDR4 */
DLLAPI int WriteMemory(UINT/*id*/,UINT/*instruction*/,UINT/*start*/,UINT/*length*/,WORD*/*pData*/);
/* Read data from DDR4 */
DLLAPI int ReadMemory(UINT/*id*/,UINT/*instruction*/,UINT/*start*/,UINT/*length*/,WORD*/*pData*/);
/* Set TCPIP timeout,uint:second. */
DLLAPI int SetTimeOut(UINT/*id*/,float/*time*/);
/* Set Task Mode, set task mode to PARALLEL mode. It is auto reset to SERIAL mode after starttask. */
DLLAPI int SetTaskMode();
/* If run as PARALLEL mode, the result will be store in stack, The stack is first in last out.*/
DLLAPI int GetReturn(UINT/*id*/,WORD*/*pData*/,UINT /*isStart*/);
/* Launch Device thread to execute Task. */
DLLAPI int StartTask();
/* Check whether the task execute finished. */
DLLAPI int CheckFinished(UINT* /*isFinished*/);

#endif