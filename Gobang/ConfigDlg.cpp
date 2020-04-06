/*
**************************************************
* 文件名：ConfigDlg.cpp
* 描  述：
* 作  者：Walnut
* 日  期：2010-9-9 21:11:17
**************************************************
*/

// ConfigDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Gobang.h"
#include "ConfigDlg.h"
#include <Winsock2.h>

#include "LevelDlg.h"
#include "NetworkDlg.h"
// CConfigDlg dialog

IMPLEMENT_DYNAMIC(CConfigDlg, CDialog)

CConfigDlg::CConfigDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CConfigDlg::IDD, pParent)
	, m_nMode(MODE_ERROR)
	, m_dwServerIPAddress(0)
	, m_nPort(8000)
	, m_nLevel(LEVEL_ERROR)
	, m_nChessmanColor(RGB(255,255,255))
	, m_nBoardColor(RGB(0xE4,0xC3,0x92))
	, m_strPlayerName(_T(""))
{
}

CConfigDlg::~CConfigDlg()
{
}

void CConfigDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_PLAYER_NAME, m_strPlayerName);
}


BEGIN_MESSAGE_MAP(CConfigDlg, CDialog)
	ON_BN_CLICKED(IDC_RADIO_CHESSMAN_WHITE, &CConfigDlg::OnBnClickedRadioChessmanWhite)
	ON_BN_CLICKED(IDC_RADIO_CHESSMAN_BLACK, &CConfigDlg::OnBnClickedRadioChessmanBlack)
	ON_BN_CLICKED(IDC_RADIO_MODE_SINGLE, &CConfigDlg::OnBnClickedRadioModeSingle)
	ON_BN_CLICKED(IDC_RADIO_MODE_DOUBLE, &CConfigDlg::OnBnClickedRadioModeDouble)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BUTTON_SET_BACKGROUND_COLOR, &CConfigDlg::OnBnClickedButtonSetBackgroundColor)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON_FINISH, &CConfigDlg::OnBnClickedButtonFinish)
	ON_BN_CLICKED(IDC_BUTTON_CANCLE, &CConfigDlg::OnBnClickedButtonCancle)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, &CConfigDlg::OnBnClickedButtonApply)
END_MESSAGE_MAP()


// CConfigDlg message handlers
BOOL CConfigDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	if(MODE_SINGLE == this->m_nMode)
	{
		((CButton*)this->GetDlgItem(IDC_RADIO_MODE_SINGLE))->SetCheck(1);
		((CButton*)this->GetDlgItem(IDC_RADIO_MODE_DOUBLE))->SetCheck(0);
	}
	else if(MODE_ERROR != this->m_nMode)
	{
		((CButton*)this->GetDlgItem(IDC_RADIO_MODE_DOUBLE))->SetCheck(1);
		((CButton*)this->GetDlgItem(IDC_RADIO_MODE_SINGLE))->SetCheck(0);
	}

	if(RGB(255,255,255) == this->m_nChessmanColor)
	{
		((CButton*)this->GetDlgItem(IDC_RADIO_CHESSMAN_WHITE))->SetCheck(1);
		((CButton*)this->GetDlgItem(IDC_RADIO_CHESSMAN_BLACK))->SetCheck(0);
	}
	else if(RGB(0,0,0) == this->m_nChessmanColor)
	{
		((CButton*)this->GetDlgItem(IDC_RADIO_CHESSMAN_BLACK))->SetCheck(1);
		((CButton*)this->GetDlgItem(IDC_RADIO_CHESSMAN_WHITE))->SetCheck(0);
	}

	this->m_strPlayerName = "Player1";
	UpdateData(false);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CConfigDlg::OnBnClickedRadioChessmanWhite()
{
	// TODO: Add your control notification handler code here
	this->m_nChessmanColor = RGB(255,255,255);
}

void CConfigDlg::OnBnClickedRadioChessmanBlack()
{
	// TODO: Add your control notification handler code here
	this->m_nChessmanColor = RGB(0,0,0);
}

void CConfigDlg::OnBnClickedRadioModeSingle()
{
	// TODO: Add your control notification handler code here
	CLevelDlg lvdlg;
	lvdlg.m_nLevel = this->m_nLevel;

	if(IDC_BUTTON_CANCLE == lvdlg.DoModal()) 
	{
		if(MODE_SINGLE == this->m_nMode)
		{
			((CButton*)this->GetDlgItem(IDC_RADIO_MODE_SINGLE))->SetCheck(1);
			((CButton*)this->GetDlgItem(IDC_RADIO_MODE_DOUBLE))->SetCheck(0);
		}
		else if(MODE_ERROR != this->m_nMode)
		{
			((CButton*)this->GetDlgItem(IDC_RADIO_MODE_DOUBLE))->SetCheck(1);
			((CButton*)this->GetDlgItem(IDC_RADIO_MODE_SINGLE))->SetCheck(0);
		}
		return;
	}
	this->m_nLevel = lvdlg.m_nLevel;
	this->m_nMode = MODE_SINGLE;
}

void CConfigDlg::OnBnClickedRadioModeDouble()
{
	// TODO: Add your control notification handler code here
	CNetworkDlg nwdlg;
	nwdlg.m_nMode = this->m_nMode;
	nwdlg.m_dwServerIPAddress = this->m_dwServerIPAddress;
	nwdlg.m_nPort = this->m_nPort;

	if(IDC_BUTTON_CANCLE == nwdlg.DoModal())
	{
		if(MODE_SINGLE == this->m_nMode)
		{
			((CButton*)this->GetDlgItem(IDC_RADIO_MODE_SINGLE))->SetCheck(1);
			((CButton*)this->GetDlgItem(IDC_RADIO_MODE_DOUBLE))->SetCheck(0);
		}
		else if(MODE_ERROR != this->m_nMode)
		{
			((CButton*)this->GetDlgItem(IDC_RADIO_MODE_DOUBLE))->SetCheck(1);
			((CButton*)this->GetDlgItem(IDC_RADIO_MODE_SINGLE))->SetCheck(0);
		}
		return;
	}
	this->m_nMode = nwdlg.m_nMode;
	this->m_dwServerIPAddress = nwdlg.m_dwServerIPAddress;
	this->m_nPort = nwdlg.m_nPort;
}

HBRUSH CConfigDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  Change any attributes of the DC here
	if(nCtlColor == CTLCOLOR_STATIC && IDC_STATIC_BACKGROUND_COLOR == pWnd->GetDlgCtrlID())
	{
		pDC->SetBkColor(this->m_nBoardColor);
		hbr = (HBRUSH)::CreateSolidBrush(this->m_nBoardColor);
	}

	// TODO:  Return a different brush if the default is not desired
	return hbr;
}

void CConfigDlg::OnBnClickedButtonSetBackgroundColor()
{
	// TODO: Add your control notification handler code here
	CColorDialog cldlg(this->m_nBoardColor, CC_FULLOPEN);
	cldlg.DoModal();
	this->m_nBoardColor = cldlg.GetColor();
	this->Invalidate();
}

void CConfigDlg::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	this->OnBnClickedButtonCancle();
	//CDialog::OnClose();
}

void CConfigDlg::OnBnClickedButtonFinish()
{
	// TODO: Add your control notification handler code here
	if(0 == ((CButton*)this->GetDlgItem(IDC_RADIO_CHESSMAN_WHITE))->GetCheck() && 0 == ((CButton*)this->GetDlgItem(IDC_RADIO_CHESSMAN_BLACK))->GetCheck())
	{
		AfxMessageBox("请选择棋子颜色!");
		return;
	}
	if(0 == ((CButton*)this->GetDlgItem(IDC_RADIO_MODE_SINGLE))->GetCheck() && 0 == ((CButton*)this->GetDlgItem(IDC_RADIO_MODE_DOUBLE))->GetCheck())
	{
		AfxMessageBox("请选择游戏模式!");
		return;
	}
	UpdateData();

	this->EndDialog(IDC_BUTTON_FINISH);
}

void CConfigDlg::OnBnClickedButtonCancle()
{
	// TODO: Add your control notification handler code here
	this->EndDialog(IDC_BUTTON_CANCLE);
}

void CConfigDlg::OnBnClickedButtonApply()
{
	// TODO: Add your control notification handler code here
	if(0 == ((CButton*)this->GetDlgItem(IDC_RADIO_CHESSMAN_WHITE))->GetCheck() && 0 == ((CButton*)this->GetDlgItem(IDC_RADIO_CHESSMAN_BLACK))->GetCheck())
	{
		AfxMessageBox("请选择棋子颜色!");
		return;
	}
	if(0 == ((CButton*)this->GetDlgItem(IDC_RADIO_MODE_SINGLE))->GetCheck() && 0 == ((CButton*)this->GetDlgItem(IDC_RADIO_MODE_DOUBLE))->GetCheck())
	{
		AfxMessageBox("请选择游戏模式!");
		return;
	}
}
