
// 0628Client.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������
#include "TCPKernel.h"

// CMy0628ClientApp:
// �йش����ʵ�֣������ 0628Client.cpp
//

class CMy0628ClientApp : public CWinApp
{
public:
	CMy0628ClientApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
public:
	IKernel *m_pKernel;
	virtual int ExitInstance();
};

extern CMy0628ClientApp theApp;