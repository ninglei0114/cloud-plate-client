#pragma once
#include "afxcmn.h"
#include "md5.h"
#include <map>

// CDlgMain �Ի���--key �ļ���+�ļ�MD5
//�ļ�·�����ļ�ָ�룩�ļ�λ��  �ļ�id  �ļ���С
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
	CDlgMain(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgMain();

// �Ի�������
	enum { IDD = IDD_DLG_MAIN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

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
