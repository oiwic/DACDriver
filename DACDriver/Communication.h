/*
	FileName:Communication.h
	Author:GuoCheng
	E-mail:fortune@mail.ustc.edu.cn
	All right reserved @ GuoCheng.
	Modified: 2017.2.15
	Description:
*/

#pragma once

//Define execute unit
int RWInstructionExe(SOCKET* pSocket,CtrlCmd ctrlCmd,Resp *pResp,char *pData);	//Send a command to FPGA.
int WriteMemoryExe(SOCKET* pSocket,CtrlCmd ctrlCmd,Resp *pResp,char *pData);	//Write Memory of DDR4.
int ReadMemoryExe(SOCKET* pSocket,CtrlCmd ctrlCmd,Resp *pResp,char *pData);		//Read Memory of DDR4.
