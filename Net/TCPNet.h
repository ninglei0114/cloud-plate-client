#ifndef TCPNET_H
#define TCPNET_H

#include "INet.h"
#include "Packdef.h"
#include "IKernel.h"
#include <process.h>
class TCPNet :public CNet
{
public:
	TCPNet(IKernel *pKernel);
	~TCPNet();
public:
	 bool InitNetWork();
	 void UnInitNetWork();
	 bool SendData(char* szbuf,int nLen);
public:
	static  unsigned  __stdcall ThreadProc( void * );
public:
	SOCKET m_sockClient;
	HANDLE m_hThread;
	bool   m_bFlagQuit;
	IKernel *m_pKernel;
};



#endif