#pragma once
#include "afxcmn.h"
#include "md5.h"
#include <map>

// CDlgMain 对话框--key 文件名+文件MD5
//文件路径（文件指针）文件位置  文件id  文件大小
struct  UpLoadFileInfo
{
	 FILE * m_pfile;
	 INT64   m_nPos;
	 INT64  m_fileid;
	 INT64  m_filesize;
};

struct  DownLoadFileInfo
{
	 FILE * m_pfile;
	 INT64  m_nPos;
	 INT64  m_fileid;
	 INT64  m_filesize;
};


class CDlgMain : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgMain)

public:
	CDlgMain(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgMain();

// 对话框数据
	enum { IDD = IDD_DLG_MAIN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
	LRESULT GetFileListMsg(WPARAM W,LPARAM L);
	LRESULT onShareLink(WPARAM W,LPARAM L);
public:
	INT64 m_userid;
	virtual BOOL OnInitDialog();
	CListCtrl m_lstctrl;
	afx_msg void OnBnClickedButton1();
	std::string FileDigest(const std::string &file);
	std::map<CString,UpLoadFileInfo*>   m_mapFileNameToFileInfo;
	std::map<CString,DownLoadFileInfo*>   m_mapFileNameToFileDownLoadInfo;
	std::map<INT64 ,  DownLoadFileInfo*> m_MapFileIdToDownloadFileInfo;
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton4();
	ULONGLONG m_editLinkCode;
	afx_msg void OnBnClickedButton5();
};
