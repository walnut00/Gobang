// NetworkDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Gobang.h"
#include "NetworkDlg.h"


// CNetworkDlg dialog

IMPLEMENT_DYNAMIC(CNetworkDlg, CDialog)

CNetworkDlg::CNetworkDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNetworkDlg::IDD, pParent)
	, m_nMode(MODE_ERROR)
	, m_dwServerIPAddress(0)
	, m_nPort(8000)
{

}

CNetworkDlg::~CNetworkDlg()
{
}

void CNetworkDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_PORT, m_nPort);
	DDV_MinMaxUInt(pDX, m_nPort, 0, 65535);
}


BEGIN_MESSAGE_MAP(CNetworkDlg, CDialog)

	ON_BN_CLICKED(IDC_CHECK_SERVER_MODE, &CNetworkDlg::OnBnClickedCheckServerMode)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDOK, &CNetworkDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_CANCLE, &CNetworkDlg::OnBnClickedButtonCancle)
END_MESSAGE_MAP()


BOOL CNetworkDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here

	((CIPAddressCtrl *)GetDlgItem(IDC_IPADDRESS_SERVER))->SetAddress(this->m_dwServerIPAddress);
	if(MODE_SERVER == this->m_nMode)
	{
		((CButton*)this->GetDlgItem(IDC_CHECK_SERVER_MODE))->SetCheck(1);
		GetDlgItem(IDC_IPADDRESS_SERVER)->EnableWindow(0);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CNetworkDlg::OnBnClickedCheckServerMode()
{
	// TODO: Add your control notification handler code here
	if(0 == IsDlgButtonChecked(IDC_CHECK_SERVER_MODE)) //not checked
	{
		GetDlgItem(IDC_IPADDRESS_SERVER)->EnableWindow(true);
		//((CIPAddressCtrl *)GetDlgItem(IDC_IPADDRESS_SERVER))->ClearAddress();
	}
	else
	{
		DWORD nIP;
		if(!GetLocalIP(&nIP, NULL))
		{
			AfxMessageBox("Failed to get local IP address!");
			return;
		}

		GetDlgItem(IDC_IPADDRESS_SERVER)->EnableWindow(false);
		((CIPAddressCtrl *)GetDlgItem(IDC_IPADDRESS_SERVER))->SetAddress(nIP);
	}
}

void CNetworkDlg::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	this->OnBnClickedButtonCancle();
	//CDialog::OnClose();
}

void CNetworkDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	UpdateData();
	if(((CIPAddressCtrl *)GetDlgItem(IDC_IPADDRESS_SERVER))->IsBlank())
	{
		AfxMessageBox("请输入IP地址!");
		return;
	}
	else
	{
		((CIPAddressCtrl *)GetDlgItem(IDC_IPADDRESS_SERVER))->GetAddress(m_dwServerIPAddress);
		if(0 == this->m_dwServerIPAddress)
		{
			AfxMessageBox("请输入IP地址!");
			return;
		}
		this->m_nMode = (1 == ((CButton*)this->GetDlgItem(IDC_CHECK_SERVER_MODE))->GetCheck() ? MODE_SERVER : MODE_CLIENT);
	}

	this->EndDialog(IDOK);
}

void CNetworkDlg::OnBnClickedButtonCancle()
{
	// TODO: Add your control notification handler code here
	this->EndDialog(IDC_BUTTON_CANCLE);
}
