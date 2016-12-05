#ifndef COMMUNICATION_H
#define COMMUNICATION_H

//Define execute unit
int RWInstructionExe(SOCKET* pSocket,CtrlCmd ctrlCmd,Resp *pResp,char *pData);	//Send a command to FPGA.
int WriteMemoryExe(SOCKET* pSocket,CtrlCmd ctrlCmd,Resp *pResp,char *pData);//Write Memory of DDR4.
int ReadMemoryExe(SOCKET* pSocket,CtrlCmd ctrlCmd,Resp *pResp,char *pData); //Read Memory of DDR4.

#endif