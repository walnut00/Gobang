#pragma once

#include "GobangDefine.h"
// CNetworkDlg dialog

class CNetworkDlg : public CDialog
{
	DECLARE_DYNAMIC(CNetworkDlg)

public:
	CNetworkDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CNetworkDlg();

// Dialog Data
	enum { IDD = IDD_DOUBLE_CONFIG_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedCheckServerMode();
	afx_msg void OnClose();

public:
	GAME_MODE m_nMode;
	DWORD m_dwServerIPAddress;
	UINT m_nPort;
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButtonCancle();
};
