/*
	FileName:DACDriver.h
	Author:GuoCheng
	E-mail:fortune@mail.ustc.edu.cn
	All right reserved @ GuoCheng.
	Modified: 2018.4.23
	Description: Export function.
*/

#pragma once

#ifdef DLLAPI
#else
#define DLLAPI __declspec(dllimport)
#endif


#ifndef UINT
#define UINT unsigned int
#endif

#ifndef USHORT
#define USHORT unsigned short
#endif


/* Open a device and add it to device list. */
DLLAPI int OpenDAC(UINT */*pID*/,char*/*ip*/,USHORT/*port*/);
/* Close the device and clear the data */
DLLAPI int CloseDAC(UINT/*id*/);
/* Write a command to FPGA */
DLLAPI int WriteInstruction(UINT/*id*/,int/*instructino*/,int/*para1*/,int/*para2*/);
/* Write data to DDR4 */
DLLAPI int WriteMemory(UINT/*id*/,int/*instruction*/,int/*start*/,int/*length*/,unsigned char*/*pData*/);
/* Read data from DDR4 */
DLLAPI int ReadMemory(UINT/*id*/,int/*instruction*/,int/*start*/,int/*length*/);
/* Set TCPIP timeout,uint:second. */
DLLAPI int SetTimeOut(UINT/*id*/,int /*direction*/,float/*time*/);
/* Get funtion type and parameter */
DLLAPI int GetFunctionType(UINT/*id*/,int/*offset*/,int*/*pFunctype*/,int */*pInstruction*/,int */*pPara1*/,int */*pPara2*/);
/* If run as PARALLEL mode, the result will be store in stack, The stack is first in last out.*/
DLLAPI int GetReturn(UINT/*id*/,int /*offset*/,int*/*pRespStat*/,int*/*pRespData*/,unsigned char*/*pData*/);
/* Get previous command data*/
DLLAPI int GetCommandData(UINT id,int offset, unsigned char*pData);
/* Check whether the task execute finished. */
DLLAPI int CheckFinished(UINT/*id*/,int* /*isFinished*/);
/* Wait task finished */
DLLAPI int WaitUntilFinished(UINT /*id*/,int /*time*/);
/* Get software Information*/
DLLAPI int GetSoftInformation(char */*description*/);
/* Scan the local network */
DLLAPI int ScanDevice(char *);
/* Check if all task successed. */
DLLAPI int CheckSuccessed(UINT/*id*/,int */*pIsSuccessed*/,int*/*pPosition*/);
/* Get lastest error message */
DLLAPI int GetErrorMsg(int/* errorcode */,char */* strMsg */);