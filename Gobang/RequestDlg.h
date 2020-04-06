#pragma once


// CRequestDlg dialog

class CRequestDlg : public CDialog
{
	DECLARE_DYNAMIC(CRequestDlg)

public:
	CRequestDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CRequestDlg();

// Dialog Data
	enum { IDD = IDD_REQUEST_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonSend();
	afx_msg void OnBnClickedButtonCancel();

	CString m_strMessage;
	afx_msg void OnBnClickedButtonForce();
};
