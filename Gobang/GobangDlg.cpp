// GobangDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Gobang.h"
#include "GobangDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CGobangDlg �Ի���

CGobangDlg::CGobangDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGobangDlg::IDD, pParent)
	, nX(0)
	, nY(0)
	, nGridSize(0)
	, m_pGobang(NULL)
{
	//m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON_MAIN);
}

CGobangDlg::~CGobangDlg()
{
	if(m_pGobang)
	{
		delete m_pGobang;
		m_pGobang = NULL;
	}
}

void CGobangDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CGobangDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_BN_CLICKED(IDC_BUTTON_START_GAME, &CGobangDlg::OnBnClickedButtonStartGame)
	ON_MESSAGE(UM_SHOW_MESSAGE, OnShowMessage)
	ON_BN_CLICKED(IDC_BUTTON_STEP_BACK, &CGobangDlg::OnBnClickedButtonStepBack)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_GIVE_UP, &CGobangDlg::OnBnClickedButtonGiveUp)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CGobangDlg ��Ϣ�������

BOOL CGobangDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������	
	
	CRect rect;
	GetClientRect(&rect);
	nGridSize = 30;
	nX = rect.Width();
	nY = rect.Height();
	
	//this->rectChessboard.SetRect((nX - nX/nGridSize*nGridSize) / 2, (nY - nY/nGridSize*nGridSize) / 2, (nX - nX/nGridSize*nGridSize) / 2 + nX/nGridSize*nGridSize, (nY - nY/nGridSize*nGridSize) / 2 + nY/nGridSize*nGridSize);
	this->rectChessboard.SetRect(nGridSize, nGridSize, nGridSize * (nX/nGridSize - 4), nGridSize * (nY/nGridSize -  1));
	this->m_pGobang = new CGobangGame(this, rectChessboard, nGridSize);

	SetTimer(0, 500, NULL);
	this->SetWindowTextA(this->m_pGobang->GetGameVersion());

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CGobangDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������
		CRect rect;
		GetClientRect(&rect);
		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ��������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		
		
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();

		//added code
		this->m_pGobang->GameSetup();

		RECT rectTmp;
		((CWnd*)this->GetDlgItem(IDC_STATIC_CHESSMAN_COLOR))->GetWindowRect(&rectTmp);
		CClientDC dc((CWnd*)this->GetDlgItem(IDC_STATIC_CHESSMAN_COLOR));
		
		CBrush NewBrush(this->m_pGobang->GetOpponentChessmanColor());
		CBrush * pOldBrush = dc.SelectObject(&NewBrush);
		if(!pOldBrush) return ;
		dc.Ellipse(5, 20, 35, 50);
		dc.SelectObject(pOldBrush);
		
		CBrush NewBrush1(this->m_pGobang->GetPlayerChessmanColor());
		pOldBrush = dc.SelectObject(&NewBrush1);
		if(!pOldBrush) return ;
		dc.Ellipse(5, 55, 35, 85);
		dc.SelectObject(pOldBrush);
	}	
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù����ʾ��
//
HCURSOR CGobangDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CGobangDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if(this->m_pGobang->IsGameStart()) 
	{
		this->m_pGobang->PlayerStep(point);
	}

	CDialog::OnLButtonUp(nFlags, point);
}

void CGobangDlg::SetGridWidth(int nWidth)
{
	this->nGridSize = nWidth;
	Invalidate();
}

void CGobangDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if(this->m_pGobang->IsGameStart())
	{
		this->m_pGobang->SetCursorShape(point);
	}

	CDialog::OnMouseMove(nFlags, point);
}

void CGobangDlg::OnBnClickedButtonStartGame()
{
	// TODO: Add your control notification handler code here
	UpdateData();

	this->m_pGobang->GameStart();
	this->ValidateButton(false);
	
	this->SetDlgItemTextA(IDC_STATIC_OPPONENT_NAME, this->m_pGobang->GetOpponentName());
	this->SetDlgItemTextA(IDC_STATIC_PLAYER_NAME, this->m_pGobang->GetPlayerName());

	AfxBeginThread(ThreadResult, (LPVOID)this);
}

LRESULT CGobangDlg::OnShowMessage(WPARAM wParam, LPARAM lParam)
{
	this->GetDlgItem(IDC_STATIC_SHOW_STATUS)->SetWindowTextA((LPCTSTR)lParam);
	return 0;
}

void CGobangDlg::ValidateButton(bool bEable)
{
	GetDlgItem(IDC_BUTTON_START_GAME)->EnableWindow(bEable);
	GetDlgItem(IDC_BUTTON_GIVE_UP)->EnableWindow(!bEable);
	GetDlgItem(IDC_BUTTON_STEP_BACK)->EnableWindow(!bEable);
}

void CGobangDlg::OnBnClickedButtonStepBack()
{
	// TODO: Add your control notification handler code here
	this->m_pGobang->StepBack();
}

void CGobangDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	int nID_White = (RGB(255,255,255) == this->m_pGobang->GetPlayerChessmanColor()) ? IDC_STATIC_TIME_WHITE : IDC_STATIC_TIME_BLACK;
	int nID_Black = IDC_STATIC_TIME_WHITE + IDC_STATIC_TIME_BLACK - nID_White;

	CTimeSpan time = this->m_pGobang->GetPlayerSpendingTime();
	CString strShow = time.Format("%H:%M:%S");
	this->GetDlgItem(nID_White)->SetWindowTextA(strShow);

	time = this->m_pGobang->GetOpponentSpendingTime();
	strShow = time.Format("%H:%M:%S");
	this->GetDlgItem(nID_Black)->SetWindowTextA(strShow);

	nID_White = (RGB(255,255,255) == this->m_pGobang->GetPlayerChessmanColor()) ? IDC_STATIC_COUNT_WHITE : IDC_STATIC_COUNT_BLACK;
	nID_Black = IDC_STATIC_COUNT_WHITE + IDC_STATIC_COUNT_BLACK - nID_White;

	this->SetDlgItemInt(nID_White, this->m_pGobang->GetPlayerStepCount());
	this->SetDlgItemInt(nID_Black, this->m_pGobang->GetOpponentStepCount());

	CDialog::OnTimer(nIDEvent);
}

void CGobangDlg::OnBnClickedButtonGiveUp()
{
	// TODO: Add your control notification handler code here
	 this->m_pGobang->GiveUp();
}

UINT CGobangDlg::ThreadResult( LPVOID pParam )
{
	CGobangDlg * pThis = (CGobangDlg *)pParam;
	GAME_RESULT nResult = pThis->m_pGobang->GetResult();
	while(RESULT_NONE == nResult)
	{
		Sleep(10);
		nResult = pThis->m_pGobang->GetResult();
	}

	Sleep(100);
	if((RGB(0,0,0) == pThis->m_pGobang->GetPlayerChessmanColor() && RESULT_BWIN == nResult)
		|| ((RGB(255,255,255) == pThis->m_pGobang->GetPlayerChessmanColor() && RESULT_WWIN == nResult)))
		AfxMessageBox("��ϲ�㣬��Ӯ�ˣ�");
	else if((RGB(0,0,0) == pThis->m_pGobang->GetPlayerChessmanColor() && RESULT_WWIN == nResult)
		|| ((RGB(255,255,255) == pThis->m_pGobang->GetPlayerChessmanColor() && RESULT_BWIN == nResult)))
		AfxMessageBox("�ܱ�Ǹ�������ˣ�");
	else if(RESULT_PEACE == nResult)
		AfxMessageBox("���壡");

	pThis->ValidateButton(true);
	return 0;
}

void CGobangDlg::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	if(!this->m_pGobang->Quit()) return;

	CDialog::OnClose();
}
