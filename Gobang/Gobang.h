/*
**************************************************
* �ļ�����Gobang.h
* ��  ����
* ��  �ߣ�Neil
* ��  �ڣ�2010-9-9 21:10:04
**************************************************
*/

// Gobang.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CGobangApp:
// �йش����ʵ�֣������ Gobang.cpp
//

class CGobangApp : public CWinApp
{
public:
	CGobangApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CGobangApp theApp;