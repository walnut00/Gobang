/*
**************************************************
* 文件名：ConfigDlg.h
* 描  述：
* 作  者：Walnut
* 日  期：2010-9-9 21:09:53
**************************************************
*/

#pragma once

#include "Resource.h"
#include "afxcmn.h"
#include "GobangDefine.h"
// CConfigDlg dialog

class CConfigDlg : public CDialog
{
	DECLARE_DYNAMIC(CConfigDlg)

public:
	CConfigDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CConfigDlg();

// Dialog Data
	enum { IDD = IDD_CONFIG_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedRadioChessmanWhite();	
	afx_msg void OnBnClickedRadioChessmanBlack();
	afx_msg void OnBnClickedRadioModeSingle();
	afx_msg void OnBnClickedRadioModeDouble();	
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedButtonSetBackgroundColor();
	afx_msg void OnClose();
	afx_msg void OnBnClickedButtonFinish();
	afx_msg void OnBnClickedButtonCancle();
	afx_msg void OnBnClickedButtonApply();
	virtual BOOL OnInitDialog();

public:
	GAME_MODE m_nMode;
	DWORD m_dwServerIPAddress;
	UINT m_nPort;
	GAME_LEVEL m_nLevel;	
	COLORREF m_nChessmanColor;
	COLORREF m_nBoardColor;
	CString m_strPlayerName;
};
