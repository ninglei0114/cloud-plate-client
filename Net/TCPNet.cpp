#include "stdafx.h"
#include "TCPNet.h"

TCPNet::TCPNet(IKernel *pKernel)
{
	m_sockClient = NULL;
	m_hThread = NULL;
	m_bFlagQuit = true;
	m_pKernel = pKernel;
}

TCPNet::~TCPNet()
{}


bool TCPNet::InitNetWork()
{
	//���ؿ�
   WORD wVersionRequested;
    WSADATA wsaData;
    int err;

/* Use the MAKEWORD(lowbyte, highbyte) macro declared in Windef.h */
    wVersionRequested = MAKEWORD(2, 2);

    err = WSAStartup(wVersionRequested, &wsaData);
    if (err != 0) {
      
        return false;
    }

    if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
       
       UnInitNetWork();
        return false;
    }
   
	//socket
	 m_sockClient = socket(AF_INET ,SOCK_STREAM,IPPROTO_TCP );
	 if(m_sockClient == INVALID_SOCKET)
	 {
		 UnInitNetWork();
        return false;
	 }
	//connect
	 sockaddr_in  addr;
	 addr.sin_family = AF_INET;
	 addr.sin_port = htons(_DEFAULT_PORT);
	 addr.sin_addr.S_un.S_addr =inet_addr(SERVER_IP);
	 if(SOCKET_ERROR == connect(m_sockClient,(const sockaddr*)&addr,sizeof(addr)))
	 {
		  UnInitNetWork();
          return false;
	 }

	//�����߳�--recv
	 m_hThread =(HANDLE) _beginthreadex(NULL,0,&ThreadProc,this,0,0);

	 return true;
}

unsigned  __stdcall TCPNet::ThreadProc( void * lpvoid )
{
	TCPNet *pthis = (TCPNet*)lpvoid;
	int nPackSize = 0;
	int nRelReadNum;
	char *pSzBuf = NULL;
	int noffset = 0;
	while(pthis->m_bFlagQuit)
	{
		//recv --4
	  	nRelReadNum = recv(pthis->m_sockClient,(char*)&nPackSize,sizeof(int),0);
		if(nRelReadNum<=0)
			continue;

		pSzBuf = new char[nPackSize];
		 noffset = 0;
		while(nPackSize)
		{
			nRelReadNum = recv(pthis->m_sockClient,pSzBuf+noffset,nPackSize,0);
			if(nRelReadNum >0)
			{
				noffset += nRelReadNum;
				nPackSize -= nRelReadNum;

			}
		}
		//�������ݰ�
		//���Ž���recv

		//���������ദ��
		pthis->m_pKernel->DealData(pSzBuf);


		delete []pSzBuf;
		pSzBuf = NULL;

	}
	return 0;
}

void TCPNet::UnInitNetWork()
{

	m_bFlagQuit = false;
	if(m_hThread)
	{
		if(WAIT_TIMEOUT == WaitForSingleObject(m_hThread,100))
			TerminateThread(m_hThread,-1);

		CloseHandle(m_hThread);
		m_hThread = NULL;
	}

	if(m_sockClient)
	{
		closesocket(m_sockClient);
		m_sockClient = NULL;
		
	}
	 WSACleanup();

}

bool TCPNet::SendData(char* szbuf,int nLen)
{
	if(!szbuf || nLen <=0)
		return false;

	/*if(send(m_sockClient,(const char*)&nLen,sizeof(int),0) <=0)
		return false;*/

	if(send(m_sockClient,szbuf,nLen,0) <=0)
		return false;


	return true;
}