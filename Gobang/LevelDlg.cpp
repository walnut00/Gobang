// LevelDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Gobang.h"
#include "LevelDlg.h"


// CLevelDlg dialog

IMPLEMENT_DYNAMIC(CLevelDlg, CDialog)

CLevelDlg::CLevelDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLevelDlg::IDD, pParent)
	, m_nLevel(LEVEL_ERROR)
{

}

CLevelDlg::~CLevelDlg()
{
}

void CLevelDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CLevelDlg, CDialog)
	ON_BN_CLICKED(IDC_RADIO_EASY, &CLevelDlg::OnBnClickedRadioEasy)
	ON_BN_CLICKED(IDC_RADIO_HARD, &CLevelDlg::OnBnClickedRadioHard)
	ON_BN_CLICKED(IDC_RADIO_EXPERT, &CLevelDlg::OnBnClickedRadioExpert)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDOK, &CLevelDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_CANCLE, &CLevelDlg::OnBnClickedButtonCancle)
END_MESSAGE_MAP()


// CLevelDlg message handlers

void CLevelDlg::OnBnClickedRadioEasy()
{
	// TODO: Add your control notification handler code here
	this->m_nLevel = LEVEL_EASY;
}

void CLevelDlg::OnBnClickedRadioHard()
{
	// TODO: Add your control notification handler code here
	this->m_nLevel =LEVEL_HARD;
}

void CLevelDlg::OnBnClickedRadioExpert()
{
	// TODO: Add your control notification handler code here
	this->m_nLevel = LEVEL_EXPERT;
}

void CLevelDlg::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	
	this->OnBnClickedButtonCancle();
	//CDialog::OnClose();
}

BOOL CLevelDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	if(LEVEL_EASY == this->m_nLevel)
		((CButton*)this->GetDlgItem(IDC_RADIO_EASY))->SetCheck(1);
	else if(LEVEL_HARD == this->m_nLevel)
		((CButton*)this->GetDlgItem(IDC_RADIO_HARD))->SetCheck(1);
	else if(LEVEL_EXPERT == this->m_nLevel)
		((CButton*)this->GetDlgItem(IDC_RADIO_EXPERT))->SetCheck(1);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CLevelDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	if(LEVEL_ERROR == this->m_nLevel)
	{
		AfxMessageBox("请选择电脑级别");
		return;
	}

	this->EndDialog(IDOK);
}

void CLevelDlg::OnBnClickedButtonCancle()
{
	// TODO: Add your control notification handler code here
	this->EndDialog(IDC_BUTTON_CANCLE);
}
