// JsonConvert.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CJsonConvertApp:
// �йش����ʵ�֣������ JsonConvert.cpp
//

class CJsonConvertApp : public CWinApp
{
public:
	CJsonConvertApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CJsonConvertApp theApp;