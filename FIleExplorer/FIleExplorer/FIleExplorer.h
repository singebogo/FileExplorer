
// FIleExplorer.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CFIleExplorerApp:
// �йش����ʵ�֣������ FIleExplorer.cpp
//

class CFIleExplorerApp : public CWinApp
{
public:
	CFIleExplorerApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CFIleExplorerApp theApp;