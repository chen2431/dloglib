
// dlogDemo.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CdlogDemoApp:
// �йش����ʵ�֣������ dlogDemo.cpp
//

class CdlogDemoApp : public CWinApp
{
public:
	CdlogDemoApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CdlogDemoApp theApp;