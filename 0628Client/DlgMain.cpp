// DlgMain.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "0628Client.h"
#include "DlgMain.h"
#include "afxdialogex.h"


// CDlgMain �Ի���

IMPLEMENT_DYNAMIC(CDlgMain, CDialogEx)

CDlgMain::CDlgMain(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgMain::IDD, pParent)
	, m_editLinkCode(0)
{

}

CDlgMain::~CDlgMain()
{
}

void CDlgMain::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_lstctrl);
	DDX_Text(pDX, IDC_EDIT1, m_editLinkCode);
}


BEGIN_MESSAGE_MAP(CDlgMain, CDialogEx)
	ON_MESSAGE(UM_FILELISTMSG,&CDlgMain::GetFileListMsg)
	ON_MESSAGE(UM_SHARE_LINK,&CDlgMain::onShareLink)
	ON_BN_CLICKED(IDC_BUTTON1, &CDlgMain::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON3, &CDlgMain::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CDlgMain::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON5, &CDlgMain::OnBnClickedButton5)
END_MESSAGE_MAP()


// CDlgMain ��Ϣ�������
//�������봫��MD5�����ϵ�����
LRESULT CDlgMain::GetFileListMsg(WPARAM W,LPARAM L)
{
	FILEINFO *pInfo = (FILEINFO*)W;
	TCHAR szbuf[100] = {0};
	for(int i = 0;i <L;i++)
	{
		m_lstctrl.InsertItem(i,pInfo[i].m_fileName);
		_itoa_s(pInfo[i].m_fileSize,szbuf,100,10);
		m_lstctrl.SetItemText(i,1,szbuf);
		m_lstctrl.SetItemText(i,2,pInfo[i].m_fileUpLoadTime);
		m_lstctrl.SetItemText(i,3,pInfo[i].m_fileType);
	}
	return 0;
}

LRESULT CDlgMain::onShareLink(WPARAM W,LPARAM L)
{
	INT32 nCode = *(INT32*)W ;
	m_editLinkCode = nCode ;
	UpdateData( FALSE );

	return 0;
}


BOOL CDlgMain::OnInitDialog()
{
	CDialogEx::OnInitDialog();


	m_lstctrl.InsertColumn(0,_T("filename"),0,100,-1);
	m_lstctrl.InsertColumn(1,_T("filesize"),0,100,-1);
	m_lstctrl.InsertColumn(2,_T("fileuploadtime"),0,100,-1);
	m_lstctrl.InsertColumn(3,_T("filetype"),0,100,-1);



	STRU_GETFILELIST_RQ sgr;
	sgr.m_nType = _DEF_PROTOCOL_GETFILELIST_RQ;
	sgr.m_UserId = m_userid;

	theApp.m_pKernel->SendData((char*)&sgr,sizeof(sgr));

	return TRUE;  // return TRUE unless you set the focus to a control

}

std::string CDlgMain::FileDigest(const std::string &file) 
{  
  
    std::ifstream in(file.c_str(),  std::ios::binary);  
	int n = GetLastError();
    if (!in)  
        return "";  
  
    MD5 md5;  
    std::streamsize length;  
    char buffer[1024];  
    while (!in.eof()) {  
        in.read(buffer, 1024);  
        length = in.gcount();  
        if (length > 0)  
            md5.update(buffer, length);  
    }  
    in.close();  
    return md5.toString();  
}  

void CDlgMain::OnBnClickedButton1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//ȡҪ������ļ�
	CFileDialog dlg(TRUE);
	if(IDOK == dlg.DoModal())
	{
		CString strFileName = dlg.GetFileName();
		CString strFilePath = dlg.GetPathName();
		STRU_UPLOADFILEHEADER_RQ sur;
		
		{
			CFile file(strFilePath,CFile::modeRead);

			sur.m_fileSize = file.GetLength();

		}
		sur.m_nType = _DEF_PROTOCOL_UPLOAD_FILEHEADER_RQ;
		strcpy_s(sur.m_szFileName,sizeof(sur.m_szFileName),strFileName);
		
		string strMD5 =   FileDigest(strFilePath.GetBuffer());
		strcpy_s(sur.m_szMD5,sizeof(sur.m_szMD5),strMD5.c_str());
		
		sur.m_UserId = m_userid;

		CTime t = CTime::GetCurrentTime();

		CString strTime = t.Format(_T("%Y -%m -%d  %H:%M:%S"));

		strcpy_s(sur.m_szFileUpLoadTime,sizeof(sur.m_szFileUpLoadTime),strTime);

		
		strcpy_s(sur.m_szFileType,sizeof(sur.m_szFileType),dlg.GetFileExt());

		
		UpLoadFileInfo *pInfo = new UpLoadFileInfo;
		fopen_s(&pInfo->m_pfile,strFilePath,"rb");
		pInfo->m_fileid = 0;
		pInfo->m_filesize = sur.m_fileSize;
		pInfo->m_nPos = 0;
		CString str = strFileName + sur.m_szMD5;
		m_mapFileNameToFileInfo[str] = pInfo;
		theApp.m_pKernel->SendData((char*)&sur,sizeof(sur));
		//���ļ�--��ȡ�ļ���Сr
		
		//�ļ������ļ�md5   �ļ�·�����ļ�ָ�룩�ļ�λ��  �ļ�id  �ļ���С
	}

}

//������ذ�ť
void CDlgMain::OnBnClickedButton3()
{
	//1. ui����
	//��ȡ ������ļ���Ϣ
	int index = m_lstctrl.GetSelectionMark();
	if( index == -1 ) return ;

	STRU_DOWNLOADFILEHEADER_RQ sdr ;
	sdr.m_nType = _DEF_PROTOCOL_DOWNLOAD_FILEHEADER_RQ;
	sdr.m_UserId = this->m_userid;

	CString strFileName = m_lstctrl.GetItemText( index , 0 );
	CString strFileSize = m_lstctrl.GetItemText( index , 1 );
	strcpy_s( sdr.m_szFileName , _DEF_SIZE , strFileName.GetBuffer());

	CFileDialog dlg( FALSE , 0 , strFileName ) ;

	if( IDOK != dlg.DoModal())  return ;

	CString  strFilePath = dlg.GetPathName();
	CString  strFileNameNew = dlg.GetFileName();

	//2. �洢���� map
	DownLoadFileInfo * pInfo = new  DownLoadFileInfo;
	pInfo->m_nPos = 0 ; 
	pInfo->m_fileid = 0 ;
	pInfo->m_pfile = 0 ;
	pInfo->m_filesize = _atoi64( strFileSize.GetBuffer() );//��listCtrl�л�ȡ��
	fopen_s(&pInfo->m_pfile,strFilePath.GetBuffer(),"wb" );

	m_mapFileNameToFileDownLoadInfo[ strFileName ] = pInfo;

	//3. ����rq��

	theApp.m_pKernel->SendData( (char*)&sdr , sizeof(sdr) );

}

//��������ļ�
void CDlgMain::OnBnClickedButton4()
{
	//���� rq 
	STRU_SHARELINK_RQ rq;
	rq.m_nType = _DEF_PROTOCOL_SHARELINK_RQ ;
	rq.m_UserId = this->m_userid;
	//rq.m_szFileName todo
	//�� ui ��ȡ �ļ��� 
	int nIndex = m_lstctrl.GetSelectionMark();

	if( nIndex == -1 ) return;
	CString strFileName = m_lstctrl.GetItemText(nIndex , 0);
	strcpy_s( rq.m_szFileName , _DEF_SIZE , strFileName);
	//����rq
	theApp.m_pKernel->SendData( (char*)&rq , sizeof(rq) );
}

//�����ȡ��ť
void CDlgMain::OnBnClickedButton5()
{
	UpdateData(); // ui-> ����

	STRU_GETLINK_RQ rq;
	rq.m_nType = _DEF_PROTOCOL_GETLINK_RQ;
	rq.m_UserId = this->m_userid;
	rq.m_code = m_editLinkCode;

	theApp.m_pKernel->SendData( (char*)&rq , sizeof(rq));

}
