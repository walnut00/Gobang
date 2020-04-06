/*
**************************************************
* 文件名：Gobang.h
* 描  述：
* 作  者：Neil
* 日  期：2010-9-9 21:10:04
**************************************************
*/

// Gobang.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号


// CGobangApp:
// 有关此类的实现，请参阅 Gobang.cpp
//

class CGobangApp : public CWinApp
{
public:
	CGobangApp();

// 重写
	public:
	virtual BOOL InitInstance();

// 实现

	DECLARE_MESSAGE_MAP()
};

extern CGobangApp theApp;