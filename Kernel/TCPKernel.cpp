#include "stdafx.h"
#include "TCPKernel.h"
#include "TCPNet.h"
#include "0628Client.h"
#include "DlgMain.h"

TCPKernel::TCPKernel()
{
	m_pNet = new TCPNet(this);
	
}

TCPKernel::~TCPKernel()
{
	delete m_pNet;
	m_pNet = NULL;
}

ProtocolMap m_protocolEntries[]=
{    
	{_DEF_PROTOCOL_REGISTER_RS,&TCPKernel::RegisterRs},
	{_DEF_PROTOCOL_LOGIN_RS,&TCPKernel::LoginRs},
	{_DEF_PROTOCOL_GETFILELIST_RS,&TCPKernel::GetFileListRs},
	{_DEF_PROTOCOL_UPLOAD_FILEHEADER_RS,&TCPKernel::UploadFileHeaderRs},
	{_DEF_PROTOCOL_UPLOAD_FILEBLOCK_RS,&TCPKernel::UploadFileBlockRs},
	{_DEF_PROTOCOL_DOWNLOAD_FILEHEADER_RS,&TCPKernel::DownloadFileHeaderRs},
	{_DEF_PROTOCOL_DOWNLOAD_FILEBLOCK_RQ,&TCPKernel::DownloadFileBlockRq},
	{_DEF_PROTOCOL_SHARELINK_RS,&TCPKernel::ShareLinkRs},
	{_DEF_PROTOCOL_GETLINK_RS,&TCPKernel::GetLinkRs},
	{0,0}

};

void TCPKernel::UploadFileBlockRs(char *szbuf)
{
	STRU_UPLOADFILEBLOCK_RS *psur = (STRU_UPLOADFILEBLOCK_RS*)szbuf;
	auto ite = ((CDlgMain*)theApp.m_pMainWnd)->m_mapFileNameToFileInfo.begin();
	STRU_UPLOADFILEBLOCK_RQ sur;
	sur.m_fileid = psur->m_fileid;
	sur.m_nType = _DEF_PROTOCOL_UPLOAD_FILEBLOCK_RQ;
	sur.m_UserId = ((CDlgMain*)theApp.m_pMainWnd)->m_userid;
	UpLoadFileInfo *pInfo = NULL;
	while(ite !=((CDlgMain*)theApp.m_pMainWnd)->m_mapFileNameToFileInfo.end() )
	{
		if(ite->second->m_fileid == psur->m_fileid)
		{
			pInfo = ite->second;
			break;
		}
		ite++;
	}

	pInfo->m_nPos += psur->m_fileNum;

	if(pInfo->m_nPos == pInfo->m_filesize)
	{
		//文件发送完成
		//向窗口发送消息  ---文件名 大小  上传
		fclose(pInfo->m_pfile);

		delete pInfo;
		pInfo = NULL;
		ite = ((CDlgMain*)theApp.m_pMainWnd)->m_mapFileNameToFileInfo.erase(ite);
		return;
	}
	
	sur.m_fileNum =  fread(sur.m_szFileContent,sizeof(char),sizeof(sur.m_szFileContent),pInfo->m_pfile);

	m_pNet->SendData((char*)&sur,sizeof(sur));
	//100      
}

//client  200  -- server 200

void TCPKernel::UploadFileHeaderRs(char *szbuf)
{
	STRU_UPLOADFILEHEADER_RS *psur = (STRU_UPLOADFILEHEADER_RS*)szbuf;
	  //读取文件内容，并传输
	STRU_UPLOADFILEBLOCK_RQ sur;
	sur.m_nType = _DEF_PROTOCOL_UPLOAD_FILEBLOCK_RQ;
	sur.m_UserId = psur->m_UserId;
	sur.m_fileid = psur->m_fileid;
	
	CString str = psur->m_szFileName;
	str += psur->m_szMD5;
	UpLoadFileInfo *pInfo = ((CDlgMain*)theApp.m_pMainWnd)->m_mapFileNameToFileInfo[str];
   
	// 如果是断点续传的话
	if(psur->m_szResult == _uploadfileheader_continue)
	{
		//将文件指针移到上次断开的位置
		_fseeki64(pInfo->m_pfile,psur->m_nPos,SEEK_SET );
	}
	else if(psur->m_szResult == _uploadfileheader_transfer)
	{
		//将文件信息直接显示到窗口上
	}

	pInfo->m_fileid = psur->m_fileid;
	pInfo->m_nPos = psur->m_nPos;

	sur.m_fileNum =  fread_s(sur.m_szFileContent,sizeof(sur.m_szFileContent),sizeof(char),sizeof(sur.m_szFileContent),pInfo->m_pfile);
	


	m_pNet->SendData((char*)&sur,sizeof(sur));
}

void TCPKernel::DownloadFileHeaderRs(char *szbuf)
{
	//1.拆包 , 查看rs的结果
	STRU_DOWNLOADFILEHEADER_RS *rs = (STRU_DOWNLOADFILEHEADER_RS *)szbuf;

	CDlgMain * pDlg =  (CDlgMain * )theApp.m_pMainWnd ;
	DownLoadFileInfo *pInfo = 0 ;
	//if( rs->m_szResult == _downloadfileheader_success)
	//{//结果为1 , 将信息添加到Map , 以文件id 为key
	//	auto ite =  pDlg->m_mapFileNameToFileDownLoadInfo.find(  rs->m_szFileName );
	//	if( ite != pDlg->m_mapFileNameToFileDownLoadInfo.end() )
	//	{
	//		  pInfo =  ite->second;
	//		  m_MapFileIdToDownloadFileInfo[rs->m_fileid] = pInfo;
	//	}
	//}else
	//{//结果为0 , 关闭文件, 删除节点.
	//	auto ite =  pDlg->m_mapFileNameToFileDownLoadInfo.find(  rs->m_szFileName );
	//	if( ite != pDlg->m_mapFileNameToFileDownLoadInfo.end() )
	//	{
	//		  pInfo =  ite->second;
	//		   //...
	//	}
	//}

	auto ite =  pDlg->m_mapFileNameToFileDownLoadInfo.find(  rs->m_szFileName );
	if( ite != pDlg->m_mapFileNameToFileDownLoadInfo.end() )
	{
			pInfo =  ite->second;
			if( rs->m_szResult == _downloadfileheader_success)
			{//结果为1 , 将信息添加到Map , 以文件id 为key
				pInfo->m_fileid = rs->m_fileid ; 
				pDlg->m_MapFileIdToDownloadFileInfo[rs->m_fileid] = pInfo;
			}else
			{//结果为0 , 关闭文件, 删除节点.

				fclose( pInfo->m_pfile );
				delete ite->second;ite->second = 0;
				pDlg->m_mapFileNameToFileDownLoadInfo.erase(ite);
			}
	}

}

void TCPKernel::DownloadFileBlockRq(char *szbuf)
{
	//1. 拆包 , 查找map 找对应info
	STRU_DOWNLOADFILEBLOCK_RQ *rq = (STRU_DOWNLOADFILEBLOCK_RQ *)szbuf;
	STRU_DOWNLOADFILEBLOCK_RS rs; 
	rs.m_fileid = rq->m_fileid;
	rs.m_nType = _DEF_PROTOCOL_DOWNLOAD_FILEBLOCK_RS;
	rs.m_fileNum = 0;
	rs.m_szResult = _downloadfileblock_fail ;
	DownLoadFileInfo *pInfo = 0;
	INT64 nLen = 0 ;
	CDlgMain * pDlg =  (CDlgMain * )theApp.m_pMainWnd ;
	auto ite = pDlg->m_MapFileIdToDownloadFileInfo.find(rq->m_fileid);
	if( ite != pDlg->m_MapFileIdToDownloadFileInfo.end() )
	{
		rs.m_szResult = _downloadfileblock_success;
		//写入
		pInfo = ite->second ;

		nLen = fwrite(rq->m_szFileContent,sizeof(char) ,rq->m_fileNum , pInfo->m_pfile );
		
		rs.m_fileNum = nLen;

		pInfo->m_nPos += nLen;
		//文件结束关闭 
		if(  rq->m_fileNum < _DEF_FILECONTENTLEN || pInfo->m_nPos >=  pInfo->m_filesize )
		{
			//关闭文件 , 删除节点
			fclose( pInfo->m_pfile);
			delete ite->second ;
			ite->second = 0 ;
			pDlg->m_MapFileIdToDownloadFileInfo.erase(ite);
		}
	}else
	{
		rs.m_szResult = _downloadfileblock_fail ;
	}
	//回复
	m_pNet->SendData( (char*)&rs , sizeof(rs));
}

void TCPKernel::ShareLinkRs(char *szbuf)
{
	//拆包
	STRU_SHARELINK_RS *rs = (STRU_SHARELINK_RS *)szbuf;
	//根据结果
	if(rs->m_szResult == _share_link_success)
	{
		//rs->m_code 贴在界面上
		( (CDlgMain*)theApp.m_pMainWnd )->SendMessageA( UM_SHARE_LINK , (WPARAM)&rs->m_code , 0);
	}else
	{
		AfxMessageBox("share_fail");
	}
}
void TCPKernel::GetLinkRs(char *szbuf)
{
	STRU_GETLINK_RS *rs= (STRU_GETLINK_RS *)szbuf;

	 switch ( rs->m_szResult )
	 {
		 case _getlink_fail:
		 AfxMessageBox("提取失败");
		 break;
		 case _getlink_success:
		 AfxMessageBox("提取成功");
		 break;
		 case _link_getted:
		 AfxMessageBox("已经提取过的");
		 break;
		 case _link_myself:
		 AfxMessageBox("自己分享的文件, 无法提取");
		 break;
	 }

}


void TCPKernel::GetFileListRs(char *szbuf)
{
	STRU_GETFILELIST_RS *psgr = (STRU_GETFILELIST_RS*)szbuf;

	theApp.m_pMainWnd->SendMessage(UM_FILELISTMSG,(WPARAM)psgr->m_aryFileInfo,psgr->m_nNum);

}

void TCPKernel::RegisterRs(char *szbuf)
{
	STRU_REGISTER_RS *psrr =(STRU_REGISTER_RS*)szbuf;
	char *pszResult = _T("_register_fail");
	if(psrr->m_szResult == _register_success)
		pszResult = _T("_register_success");

	AfxMessageBox(pszResult);




}

void TCPKernel::LoginRs(char *szbuf)
{
	 STRU_LOGIN_RS *psrr =(STRU_LOGIN_RS*)szbuf;
	char *pszResult = _T("_login_fail");
	if(psrr->m_szResult == _login_success)
	{
		//pszResult = _T("_login_success");

		theApp.m_pMainWnd->PostMessage(UM_LOGINMSG);
		return;
	}

	AfxMessageBox(pszResult);

}
	

bool TCPKernel::Open()
{
	if(!m_pNet->InitNetWork())
		return false;
	return true;
}
void TCPKernel::Close()
{
	m_pNet->UnInitNetWork();
}

bool TCPKernel::DealData(char *szbuf)
{
	//校验包类型
	PackType *pType = (PackType *)szbuf;
	int i =0;
	while(1)
	{
		if(*pType == m_protocolEntries[i].m_nType)
		{
			(this->*m_protocolEntries[i].m_pfun)(szbuf);
			break;
		}
		else if( m_protocolEntries[i].m_nType  == 0&&m_protocolEntries[i].m_pfun ==0)
			break;
		i++;
	}


	return true;
}

 bool TCPKernel::SendData(char* szbuf,int nlen)
 {
	if(!m_pNet->SendData(szbuf,nlen))
		return false;


	 return true;
 }