
// 0628ClientDlg.h : ͷ�ļ�
//

#pragma once


// CMy0628ClientDlg �Ի���
class CMy0628ClientDlg : public CDialogEx
{
// ����
public:
	CMy0628ClientDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_MY0628CLIENT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	LRESULT LoginMsg(WPARAM W,LPARAM L);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnClose();
	ULONGLONG m_edtuserid;
	CString m_edtpassword;
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
};
