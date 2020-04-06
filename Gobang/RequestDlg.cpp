// RequestDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Gobang.h"
#include "RequestDlg.h"


// CRequestDlg dialog

IMPLEMENT_DYNAMIC(CRequestDlg, CDialog)

CRequestDlg::CRequestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRequestDlg::IDD, pParent)
	, m_strMessage(_T(""))
{

}

CRequestDlg::~CRequestDlg()
{
}

void CRequestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_MESSAGE, m_strMessage);
}


BEGIN_MESSAGE_MAP(CRequestDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_SEND, &CRequestDlg::OnBnClickedButtonSend)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, &CRequestDlg::OnBnClickedButtonCancel)
	ON_BN_CLICKED(IDC_BUTTON_FORCE, &CRequestDlg::OnBnClickedButtonForce)
END_MESSAGE_MAP()


// CRequestDlg message handlers

void CRequestDlg::OnBnClickedButtonSend()
{
	// TODO: Add your control notification handler code here
	UpdateData();

	this->EndDialog(IDC_BUTTON_SEND);
}

void CRequestDlg::OnBnClickedButtonCancel()
{
	// TODO: Add your control notification handler code here
	this->EndDialog(IDC_BUTTON_CANCEL);
}

void CRequestDlg::OnBnClickedButtonForce()
{
	// TODO: Add your control notification handler code here
	this->EndDialog(IDC_BUTTON_FORCE);
}
