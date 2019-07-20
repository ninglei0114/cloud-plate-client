#ifndef _TCPKERNEL_H

#define _TCPKERNEL_H

#include "IKernel.h"

#include "Packdef.h"
class TCPKernel;
typedef void (TCPKernel:: *PFUN)(char *);
struct ProtocolMap
{
	PackType m_nType;
	PFUN    m_pfun;
};

class TCPKernel :public IKernel
{
public:
	TCPKernel();
	virtual ~TCPKernel();
	
public:
	 bool Open();
	 void Close();
	 bool DealData(char *szbuf);
	 bool SendData(char* szbuf,int nlen);
public:
	 void RegisterRs(char *szbuf);
	 void LoginRs(char *szbuf);
	void GetFileListRs(char *szbuf);
	void UploadFileHeaderRs(char *szbuf);
	void UploadFileBlockRs(char *szbuf);
	void DownloadFileHeaderRs(char *szbuf);
	void DownloadFileBlockRq(char *szbuf);
	void ShareLinkRs(char *szbuf);
	void GetLinkRs(char *szbuf)
	
};



#endif