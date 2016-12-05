#include "Header.h"

int RWInstructionExe(SOCKET* pSocket,CtrlCmd ctrlCmd,Resp *pResp,char *pData)
{
	if(SOCKET_ERROR == send(*pSocket,(char*)&ctrlCmd,sizeof(CtrlCmd),0)) return -1;
	if(SOCKET_ERROR == recv(*pSocket,(char*)pResp,sizeof(Resp),0)) return -1;
	return 0;
}

int WriteMemoryExe(SOCKET* pSocket,CtrlCmd ctrlCmd,Resp *pResp,char *pData)
{
	if(0 != RWInstructionExe(pSocket,ctrlCmd,pResp,0))return -1;
	if(SOCKET_ERROR == send(*pSocket,pData,ctrlCmd.para2,0)) return -1;
	if(SOCKET_ERROR == recv(*pSocket,(char*)pResp,sizeof(Resp),0)) return -1;
	return 0;
}

int ReadMemoryExe(SOCKET* pSocket,CtrlCmd ctrlCmd,Resp *pResp,char *pData)
{
	if( 0 != RWInstructionExe(pSocket,ctrlCmd,pResp,0)) return -1;
	if(SOCKET_ERROR == recv(*pSocket,pData,ctrlCmd.para2,0)) return -1;
	return 0;
}