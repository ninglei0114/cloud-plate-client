
// 0628ClientDlg.h : 头文件
//

#pragma once


// CMy0628ClientDlg 对话框
class CMy0628ClientDlg : public CDialogEx
{
// 构造
public:
	CMy0628ClientDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_MY0628CLIENT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
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
