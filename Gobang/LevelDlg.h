#pragma once

#include "GobangDefine.h"

// CLevelDlg dialog

class CLevelDlg : public CDialog
{
	DECLARE_DYNAMIC(CLevelDlg)

public:
	CLevelDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CLevelDlg();

// Dialog Data
	enum { IDD = IDD_CUPMUTER_LEVEL_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedRadioEasy();
	afx_msg void OnBnClickedRadioHard();
	afx_msg void OnBnClickedRadioExpert();
	afx_msg void OnClose();
	virtual BOOL OnInitDialog();
public:
	GAME_LEVEL m_nLevel;
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButtonCancle();
};
