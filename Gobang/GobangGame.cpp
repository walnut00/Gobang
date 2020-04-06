/*
**************************************************
* 文件名：GobangGame.cpp
* 描  述：
* 作  者：Walnut
* 日  期：2010-11-22 22:24:08
**************************************************
*/

#include "StdAfx.h"
#include "GobangGame.h"
#include "ConfigDlg.h"
#include "LevelDlg.h"
#include "NetworkDlg.h"
#include "RequestDlg.h"

CGobangGame::CGobangGame(CWnd * pWnd, const CRect & rectChessboard, int nGridSize)
						: m_pMainWnd(pWnd)
						, m_nPort(8000)
						, m_strServerIPAddress("0.0.0.0")
						, m_strClientIPAddress("0.0.0.0")
						, m_strRequestReason(" ")
						, m_nMode(MODE_ERROR)
						, m_nLevel(LEVEL_ERROR)
						, m_nResult(RESULT_NONE)
						, m_bStart(false)
						, m_bStepback(false)
						, m_bPlayerGiveup(false)
						, m_bOpponentGiveup(false)
						, m_bPlayerEscape(false)
						, m_bOpponentEscape(false)
						, m_bWaitting(false)
						, m_pThreadListen(NULL)
{
	this->m_pChessboard = new CChessboard(pWnd, rectChessboard, nGridSize);
	this->m_pPlayer = new CPlayer(pWnd, nGridSize/2, RGB(255,255,255), "");
	this->m_pOpponent = new CPlayer(pWnd, nGridSize/2, RGB(0,0,0),"");
}

CGobangGame::~CGobangGame(void)
{
	if(this->m_pChessboard)
	{
		delete m_pChessboard;
		m_pChessboard = NULL;
	}

	if(this->m_pPlayer)
	{
		delete m_pPlayer;
		m_pPlayer = NULL;
	}

	if(this->m_pOpponent)
	{
		delete m_pOpponent;
		m_pOpponent = NULL;
	}

	WSACleanup();
}

void CGobangGame::GameInitialize(void)
{
	//清空走棋记录
	this->m_pPlayer->EraseStep();
	this->m_pOpponent->EraseStep();
	this->m_pMainWnd->Invalidate();
	this->m_pMainWnd->UpdateWindow();

	this->m_pPlayer->SetMyTurn(false);
	this->m_pOpponent->SetMyTurn(false);

	//计时器复位
	this->m_pPlayer->m_pTimer->Reset();
	this->m_pOpponent->m_pTimer->Reset();

	//结果复位
	this->m_nResult = RESULT_NONE;

	this->m_bStart = false;
	this->m_bStepback = false;
	this->m_bPlayerGiveup = false;
	this->m_bOpponentGiveup = false;

	this->SendShowMessage("未开始游戏");
}

void CGobangGame::GameSetup(void)
{
	//绘制棋盘线
	this->m_pChessboard->Draw();

	//重绘所有棋子
	this->m_pPlayer->RestepAll();
	this->m_pOpponent->RestepAll();
}

bool CGobangGame::Config(void)
{
	CConfigDlg cfgdlg; 

	cfgdlg.m_nMode = this->m_nMode;
	cfgdlg.m_nLevel = this->m_nLevel;
	cfgdlg.m_dwServerIPAddress = TransferIP(this->m_strServerIPAddress.GetBuffer());
	cfgdlg.m_nPort = this->m_nPort;
	cfgdlg.m_nChessmanColor = this->GetPlayerChessmanColor();
	cfgdlg.m_nBoardColor = this->GetChessboardBackgroundColor();

	if(IDC_BUTTON_CANCLE == cfgdlg.DoModal()) 
	{
		this->m_nResult = RESULT_CONFIG_ERROR;
		this->SendShowMessage("配置错误");
		return false; 
	}
	this->m_nMode = cfgdlg.m_nMode;
	this->m_nLevel = cfgdlg.m_nLevel;
	this->m_strServerIPAddress = TransferIP(cfgdlg.m_dwServerIPAddress);
	this->m_nPort = cfgdlg.m_nPort;

	this->m_pPlayer->SetPlayerName(cfgdlg.m_strPlayerName);
	this->SetPlayerChessmanColor(cfgdlg.m_nChessmanColor);
	this->SetChessboardBackgroundColor(cfgdlg.m_nBoardColor);

	return true;
}

void CGobangGame::GameOver(void)
{
	if(this->m_bStart)
	{
		this->m_bStart = false;

		switch(this->m_nResult)
		{
		case RESULT_WWIN:
			this->SendShowMessage("白方获胜");
			break;
		case RESULT_BWIN:
			this->SendShowMessage("黑方获胜");
			break;
		case RESULT_WGIVEUP:
			this->SendShowMessage("白方认输");
			break;
		case RESULT_BGIVEUP:
			this->SendShowMessage("黑方认输");
			break;
		case RESULT_WESCAPE:
			this->SendShowMessage("白方逃跑");
			break;
		case RESULT_BESCAPE:
			this->SendShowMessage("白方认输");
			break;
		case RESULT_PEACE:
			this->SendShowMessage("和解");
			break;
		case RESULT_INIT_FAIL:
			this->SendShowMessage("初始化套接字失败");
			break;
		case RESULT_BIND_FAIL:
			this->SendShowMessage("绑定失败");
			break;
		case RESULT_LISTEN_FAIL:
			this->SendShowMessage("监听失败");
			break;
		case RESULT_CONNECT_FAIL:
			this->SendShowMessage("连接失败");
			break;
		case RESULT_CREATE_FAIL:
			this->SendShowMessage("创建套接字失败");
			break;
		case RESULT_SEND_FAIL:
			this->SendShowMessage("发送消息失败");
			break;
		case RESULT_RECEIVE_FAIL:
			this->SendShowMessage("连接失败");
			break;
		case RESULT_CONFIG_ERROR:
			this->SendShowMessage("配置错误");
			break;
		case RESULT_RUNTIME_ERROR:
			this->SendShowMessage("运行时错误");
			break;
		case RESULT_NONE:
		default:
			this->SendShowMessage("");
			break;
		}

		this->m_pPlayer->SetMyTurn(false);
		this->m_pOpponent->SetMyTurn(false);

		this->m_pPlayer->m_pTimer->Pause();
		this->m_pOpponent->m_pTimer->Pause();

		this->m_pPlayer->SetPlayerName("-");
		this->m_pOpponent->SetPlayerName("-");
	}
}

bool CGobangGame::PlayerStep(const CPoint & pointPosition)
{
	int nGridSize = this->m_pChessboard->GetGridSize();
	double dValidRadius = double(nGridSize) / 3;
	CRect rectChessboard = this->m_pChessboard->GetChessboardSize();

	CPoint pt[4]; //找到pointPosition附近的棋盘线的四个交叉点
	pt[0].SetPoint(pointPosition.x - pointPosition.x % nGridSize, pointPosition.y - pointPosition.y % nGridSize);
	pt[1].SetPoint(pointPosition.x - pointPosition.x % nGridSize + nGridSize, pointPosition.y - pointPosition.y % nGridSize);
	pt[2].SetPoint(pointPosition.x - pointPosition.x % nGridSize, pointPosition.y - pointPosition.y % nGridSize + nGridSize);
	pt[3].SetPoint(pointPosition.x - pointPosition.x % nGridSize + nGridSize, pointPosition.y - pointPosition.y % nGridSize + nGridSize);
	if(0 != rectChessboard.left % nGridSize)
	{
		pt[0].Offset(rectChessboard.left, 0);
		pt[1].Offset(rectChessboard.left, 0);
		pt[2].Offset(rectChessboard.left, 0);
		pt[3].Offset(rectChessboard.left, 0);
	}
	if(0 != rectChessboard.top % nGridSize)
	{
		pt[0].Offset(0, rectChessboard.top);
		pt[1].Offset(0, rectChessboard.top);
		pt[2].Offset(0, rectChessboard.top);
		pt[3].Offset(0, rectChessboard.top);
	}

	double dDistanceMin = sqrt(pow(pointPosition.x - pt[0].x, 2.0) + pow(pointPosition.y - pt[0].y, 2.0)); 
	double dTmp = 0;
	int nPointIndex = 0;

	//找到四个点中，离pointPosition最近的那个点
	for(int n = 1; n < 4; ++n)
	{
		dTmp = sqrt(pow(pointPosition.x - pt[n].x, 2.0) + pow(pointPosition.y - pt[n].y, 2.0)); 
		nPointIndex = dTmp < dDistanceMin ? n : nPointIndex;
		dDistanceMin = dTmp < dDistanceMin ? dTmp : dDistanceMin;
	}

	if(dDistanceMin < dValidRadius 
		&& pt[nPointIndex].x >= rectChessboard.left
		&& pt[nPointIndex].x <= rectChessboard.right
		&& pt[nPointIndex].y >= rectChessboard.top
		&& pt[nPointIndex].y <= rectChessboard.bottom)  //限定在dValidRadius为半径的圆内部，才有效
	{
		if(this->IsPositionStepped(pt[nPointIndex]))return false;
		if(!this->IsPlayerTurn()) return false;
		if(!this->m_pOpponent->UnHightlightLastStep()) return false;
		if(!this->m_pPlayer->StepGo(pt[nPointIndex])) return false;
		if(!this->m_pPlayer->HightlightLastStep()) return false;
		this->SetOpponentTurn();
	}
	return true;
}	

bool CGobangGame::OpponentStep(const CPoint & pointPosition)
{
	if(!this->IsOpponentTurn()) return false;
	if(!this->m_pPlayer->UnHightlightLastStep()) return false;
	if(!this->m_pOpponent->StepGo(pointPosition)) return false;
	if(!this->m_pOpponent->HightlightLastStep()) return false;
	this->SetPlayerTurn();
	return true;
}

bool CGobangGame::IsPositionStepped(const CPoint & pointPosition) const
{
	return (this->m_pPlayer->IsPositionStepped(pointPosition) || this->m_pOpponent->IsPositionStepped(pointPosition));
}

void CGobangGame::StepBack(void)
{
	if(!this->IsGameStart())
	{
		AfxMessageBox("游戏还未开时，不能悔棋！");
		return;
	}

	if(0 == this->GetPlayerStepCount())
	{
		AfxMessageBox("还没有走棋，不能悔棋！");
		return;
	}

	if(this->m_bWaitting)
	{
		AfxMessageBox("请等待对方回应！");
		return;
	}

	if(!this->m_pPlayer->RequestStepback()) return;

	if(MODE_SINGLE == this->m_nMode)
	{
		if(this->IsPlayerTurn())
		{
			if(!this->m_pOpponent->StepBack() || !this->m_pPlayer->StepBack())
			{
				this->m_nResult = RESULT_RUNTIME_ERROR;
				this->SendShowMessage("运行时错误");
				return;
			}
			this->m_pMainWnd->Invalidate();
			this->m_pMainWnd->UpdateWindow();
		}
	}
	else if(MODE_SERVER == this->m_nMode || MODE_CLIENT == this->m_nMode)
	{
		AfxBeginThread(this->ThreadStepBack, (LPVOID)this);
	}
	else
	{
	}
}

void CGobangGame::GiveUp(void)
{
	if(!this->IsGameStart())
	{
		AfxMessageBox("游戏还未开始，不能认输！");
		return;
	}

	if(this->m_bWaitting)
	{
		AfxMessageBox("请等待对方回应！");
		return;
	}

	if(!this->m_pPlayer->RequestGiveup()) return;

	if(MODE_SINGLE == this->m_nMode)
	{
		if(RGB(255,255,255) == this->GetPlayerChessmanColor()) //执白
		{
			this->m_nResult = RESULT_WGIVEUP;
			this->SendShowMessage("白方认输");
		}
		else if(RGB(0,0,0) == this->GetPlayerChessmanColor()) //执黑
		{
			this->m_nResult = RESULT_BGIVEUP;
			this->SendShowMessage("黑方认输");
		}
		else
		{
			this->m_nResult = RESULT_CONFIG_ERROR;
			this->SendShowMessage("配置错误");
		}
	}
	else if(MODE_SERVER == this->m_nMode || MODE_CLIENT == this->m_nMode)
	{
		AfxBeginThread(this->ThreadGiveup, (LPVOID)this);
	}
	else
	{
	}
}

bool CGobangGame::Quit(void)
{
	if(this->m_bWaitting)
	{
		AfxMessageBox("请等待对方回应！");
		return false;
	}

	if(!this->IsGameStart()) return true;

	if(!this->m_pPlayer->RequestQuit()) return false;

	if(MODE_SINGLE == this->m_nMode)
	{
		this->m_bStart = false;

		if(RGB(255,255,255) == this->GetPlayerChessmanColor()) //执白
		{
			this->m_nResult = RESULT_WESCAPE;
			this->SendShowMessage("白方逃跑");
		}
		else if(RGB(0,0,0) == this->GetPlayerChessmanColor())  //执黑
		{
			this->m_nResult = RESULT_BESCAPE;
			this->SendShowMessage("黑方逃跑");
		}
		else
		{
			this->m_nResult = RESULT_CONFIG_ERROR;
			this->SendShowMessage("配置错误");
		}

		return true;
	}
	else if(MODE_SERVER == this->m_nMode || MODE_CLIENT == this->m_nMode)
	{
		CRequestDlg RequestDlg;
		int nRet = (int)RequestDlg.DoModal();
		
		if(IDC_BUTTON_SEND == nRet)
		{
			this->m_strRequestReason = RequestDlg.m_strMessage;
			if(this->m_strRequestReason.IsEmpty()) this->m_strRequestReason = " ";
			
			AfxBeginThread(this->ThreadQuit, (LPVOID)this);
			
			return false;
		}
		else if(IDC_BUTTON_FORCE == nRet)
		{
			AfxBeginThread(this->ThreadEscape, (LPVOID)this);
			Sleep(100);
			return true;
		}
		else if(IDC_BUTTON_CANCEL == nRet)
		{
			return false;
		}
		else
			return false;
	}
	else
	{
		return false;
	}
}

void CGobangGame::SetSingleMode(void)
{
	CLevelDlg lvdlg;
	lvdlg.m_nLevel = this->m_nLevel;
	lvdlg.DoModal();
	this->m_nLevel = lvdlg.m_nLevel;
	this->m_nMode = MODE_SINGLE;
}

void CGobangGame::SetDoubleMode(void)
{
	CNetworkDlg nwdlg;
	nwdlg.m_nMode = this->m_nMode;
	nwdlg.m_dwServerIPAddress = TransferIP(this->m_strServerIPAddress.GetBuffer());
	nwdlg.m_nPort = this->m_nPort;

	nwdlg.DoModal();
	this->m_nMode = nwdlg.m_nMode;
	this->m_strServerIPAddress = TransferIP(nwdlg.m_dwServerIPAddress);
	this->m_nPort = nwdlg.m_nPort;
}

CString CGobangGame::GetOpponentName(void) const
{
	return this->m_pOpponent->GetPlayerName();
}

CString CGobangGame::GetPlayerName(void) const
{
	return this->m_pPlayer->GetPlayerName();
}

void CGobangGame::SetPlayerChessmanColor(COLORREF nMyColor)
{
	this->m_pPlayer->m_pChessman->SetColor(nMyColor);

	COLORREF nTmpColor = (RGB(255,255,255) == nMyColor ? RGB(0,0,0) : RGB(255,255,255));
	this->SetOpponentChessmanColor(nTmpColor);
}

void CGobangGame::SetOpponentChessmanColor(COLORREF nMyColor)
{
	this->m_pOpponent->m_pChessman->SetColor(nMyColor);
}

COLORREF CGobangGame::GetPlayerChessmanColor(void) const
{
	return this->m_pPlayer->m_pChessman->GetColor();
}

COLORREF CGobangGame::GetOpponentChessmanColor(void) const
{
	return this->m_pOpponent->m_pChessman->GetColor();
}

void CGobangGame::SetChessboardBackgroundColor(COLORREF nMyColor)
{
	this->m_pChessboard->SetBackgroundColor(nMyColor);
}

void CGobangGame::SetChessboardBackgroundColor(void)
{
	CColorDialog cldlg(this->GetChessboardBackgroundColor(), CC_FULLOPEN);
	cldlg.DoModal();
	this->SetChessboardBackgroundColor(cldlg.GetColor());
}

COLORREF CGobangGame::GetChessboardBackgroundColor(void) const
{
	return this->m_pChessboard->GetBackgroundColor();
}

CTimeSpan CGobangGame::GetPlayerSpendingTime(void) const
{
	return this->m_pPlayer->m_pTimer->GetElapsedTime();
}

CTimeSpan CGobangGame::GetOpponentSpendingTime(void) const
{
	return this->m_pOpponent->m_pTimer->GetElapsedTime();
}

UINT CGobangGame::GetPlayerStepCount(void) const
{
	return this->m_pPlayer->GetStepCount();
}

UINT CGobangGame::GetOpponentStepCount(void) const
{
	return this->m_pOpponent->GetStepCount();
}

void CGobangGame::SetPlayerTurn(void)
{
	this->m_pPlayer->SetMyTurn();
	this->m_pOpponent->SetMyTurn(false);

	this->m_pPlayer->m_pTimer->Run();
	this->m_pOpponent->m_pTimer->Pause();

	this->SendShowMessage("自己思考中");
}

void CGobangGame::SetOpponentTurn(void)
{
	this->m_pOpponent->SetMyTurn();
	this->m_pPlayer->SetMyTurn(false);

	this->m_pPlayer->m_pTimer->Pause();
	this->m_pOpponent->m_pTimer->Run();

	this->SendShowMessage("对手思考中");
}

bool CGobangGame::IsPlayerTurn(void) const
{
	return this->m_pPlayer->IsMyTurn();
}

bool CGobangGame::IsOpponentTurn(void) const
{
	return this->m_pOpponent->IsMyTurn();
}

GAME_RESULT CGobangGame::GetResult(void) const
{
	return this->m_nResult;
}

bool CGobangGame::IsGameStart() const
{
	return this->m_bStart;
}

CString CGobangGame::GetGameVersion(void) const
{
	CString strVersion = "Gobang Game 2.0.0";
	return strVersion;
}

void CGobangGame::GameStart(void)
{
	this->GameInitialize();
	if(!this->Config()) return;

	if(MODE_SINGLE == this->m_nMode)
	{
		if(LEVEL_EASY == this->m_nLevel)
		{
			this->SendShowMessage("游戏开始");
			AfxBeginThread(ThreadSingle, (LPVOID)this);
			AfxBeginThread(ThreadHightlight, (LPVOID)this);
		}
		else
		{
			this->m_nResult = RESULT_CONFIG_ERROR;
			this->SendShowMessage("配置错误");
			AfxMessageBox("对不起，目前只实现了简单版本！请重新选择电脑级别！");
		}
	}
	else if(MODE_SERVER == this->m_nMode)
	{
		this->SendShowMessage("等待连接...");

		AfxBeginThread(ThreadServer, (LPVOID)this);
	}
	else if(MODE_CLIENT == this->m_nMode)
	{
		this->SendShowMessage("连接服务器...");
		AfxBeginThread(ThreadClient, (LPVOID)this);
	}
	else
	{
		this->m_nResult = RESULT_CONFIG_ERROR;
		this->SendShowMessage("配置错误");
	}
}

UINT CGobangGame::GameSingle(void)
{
	int nDefendSocre = 0, nAttackSocre = 0;
	CPoint ptDefendPoint, ptAttackPoint;
	ptDefendPoint = ptAttackPoint = CPoint(-1, -1);

	this->m_bStart = true;	

	this->m_pOpponent->SetPlayerName("电脑");
	if(RGB(255,255,255) == this->GetOpponentChessmanColor()) //电脑先走一步
	{
		this->SetOpponentTurn();		

		int x = this->m_pChessboard->GetWidth();
		int y = this->m_pChessboard->GetHeight();
		int i = this->m_pChessboard->GetGridSize();
		CPoint pt;
		pt.x = (x / i / 2 + 1)  * i;
		pt.y = (y / i / 2 + 1) * i;

		this->OpponentStep(pt);
	}

	this->SetPlayerTurn();

	while(RESULT_NONE == this->m_nResult)
	{
		if(this->IsOpponentTurn())//电脑走棋
		{
			this->Analyze(this->m_pPlayer, &nDefendSocre, &ptDefendPoint);
			this->Analyze(this->m_pOpponent, &nAttackSocre, &ptAttackPoint);

			if(nAttackSocre <= 0)
			{this->OpponentStep(ptDefendPoint);}
			else
			{
				if(nAttackSocre < nDefendSocre && nDefendSocre > SOCRE_SINGLE_4/*(SOCRE_DOUBLE_3+SOCRE_DOUBLE_2)*/)
				{this->OpponentStep(ptDefendPoint);}
				else
				{this->OpponentStep(ptAttackPoint);}
			}

			this->RefreshResult(this->m_pOpponent);
		}
		else//自己走棋
		{
			Sleep(10);
			this->RefreshResult(this->m_pPlayer);
		}
	}

	this->GameOver();
	return 0;
}

UINT CGobangGame::GameServer(void)
{
	if(!AfxSocketInit())
	{
		AfxMessageBox("Failed to initialize socket!");
		this->m_nResult = RESULT_INIT_FAIL;
		this->SendShowMessage("初始化socket失败");
		return 0;
	}

	CSocket Server; //= new CSocket;
	if(0 == Server.Create(this->m_nPort, SOCK_STREAM, this->m_strServerIPAddress))
	{
		AfxMessageBox("Failed to create socket!");	
		this->m_nResult = RESULT_CREATE_FAIL;	
		this->SendShowMessage("创建socket失败");
		WSACleanup();
		return 0;
	}

	if(0 == Server.Listen())
	{
		AfxMessageBox("Failed to listen!");	
		this->m_nResult = RESULT_LISTEN_FAIL;
		this->SendShowMessage("监听失败");
		Server.Close();
		WSACleanup();
		return 0;
	}

	CSocket sockConnect;
	if(0 == Server.Accept(sockConnect))
	{
		AfxMessageBox("Failed to accept!");	
		this->m_nResult = RESULT_CONNECT_FAIL;
		this->SendShowMessage("接受连接失败");
		Server.Close();
		WSACleanup();
		return 0;
	}

	Server.Close();

	this->SendShowMessage("已建立连接");

	char cBuf[128];
	memset(cBuf, 0, 128);
	sprintf_s(cBuf, 128, "%d", this->GetPlayerChessmanColor());//发送自己棋子的颜色
	if(SOCKET_ERROR  == sockConnect.Send(cBuf, 128))
	{
		AfxMessageBox("Failed to send data!");
		this->m_nResult = RESULT_SEND_FAIL;
		this->SendShowMessage("发送数据失败");
		sockConnect.Close();
		WSACleanup();
		return 0;
	}

	memset(cBuf, 0, 128);
	if(0 == sockConnect.Receive(cBuf, 128))//接受对方的棋子颜色
	{
		AfxMessageBox("Failed to receive data!");
		this->m_nResult = RESULT_RECEIVE_FAIL;
		this->SendShowMessage("接收数据失败");
		sockConnect.Close();
		WSACleanup();
		return 0;
	}

	UINT nRemotePort;
	if(0 == sockConnect.GetPeerName(this->m_strClientIPAddress, nRemotePort))
	{
		AfxMessageBox("Failed to get remote IP address!");
		this->m_nResult = RESULT_RECEIVE_FAIL;
		this->SendShowMessage("接收数据失败");
		sockConnect.Close();
		WSACleanup();
		return 0;
	}

	COLORREF nTmpColor = (COLORREF)atoi(cBuf);
	if( (RGB(255,255,255) == this->GetPlayerChessmanColor() && RGB(0,0,0) != nTmpColor)
		|| (RGB(0,0,0) == this->GetPlayerChessmanColor() && RGB(255,255,255)  != nTmpColor))
	{
		AfxMessageBox("棋子颜色配置错误，请重新开始游戏！");
		this->m_nResult = RESULT_CONFIG_ERROR;
		this->SendShowMessage("配置错误");
		sockConnect.Close();
		WSACleanup();
		return 0;
	}
	this->SetOpponentChessmanColor(nTmpColor);//设置对方棋子颜色

	if(SOCKET_ERROR  == sockConnect.Send("START", sizeof("START")))
	{
		AfxMessageBox("Failed to send data!");
		this->m_nResult = RESULT_SEND_FAIL;
		this->SendShowMessage("发送数据");
		sockConnect.Close();
		WSACleanup();
		return 0;
	}

	memset(cBuf, 0, 128);
	if(0 == sockConnect.Receive(cBuf, 128))
	{
		AfxMessageBox("Failed to receive data!");
		this->m_nResult = RESULT_RECEIVE_FAIL;
		this->SendShowMessage("接收数据");
		sockConnect.Close();
		WSACleanup();
		return 0;
	}

	if(0 != strcmp(cBuf, "START"))
	{
		AfxMessageBox("Failed to receive start command!");
		this->m_nResult = RESULT_CONFIG_ERROR;
		this->SendShowMessage("配置错误");
		sockConnect.Close();
		WSACleanup();
		return 0;
	}

	this->SendShowMessage("开始游戏");

	this->m_bStart = true;
	if(NULL == m_pThreadListen) m_pThreadListen = AfxBeginThread(ThreadListen, (LPVOID)this);
	AfxBeginThread(this->ThreadHightlight, (LPVOID)this);

	if(RGB(255,255,255) == this->GetPlayerChessmanColor())//自己的颜色为白色
	{
		this->SetPlayerTurn();//自己下
		this->SendShowMessage("白方思考中");
	}
	else//自己的颜色为黑色
	{
		this->SetOpponentTurn();//对方下
		this->SendShowMessage("黑方思考中");
	}
	while(RESULT_NONE == this->m_nResult)
	{
		if(this->IsOpponentTurn())//对手下
		{
			if(this->m_bStepback)
			{	
#ifdef _DEBUG
				this->WriteMsg("服务线程 - 轮到自己下时，对手请求悔棋，向对方发送(-1,-1)");
#endif
				//执行到此，表示轮到自己下时，对手请求悔棋。悔棋后又轮到对手下
				CPoint ptTmp = CPoint(-1, -1);//因为对手悔棋时，轮到自己下，对方线程处在接收阻塞状态，发送(-1,-1)，进入其线程处于发送数据状态
				memset(cBuf, 0, 128);
				sprintf_s(cBuf, 128, "%d", ptTmp.x * 65535 + ptTmp.y);
				if(SOCKET_ERROR  == sockConnect.Send(cBuf, 128))
				{
					AfxMessageBox("Failed to send data!");
					this->m_nResult = RESULT_SEND_FAIL;
					this->SendShowMessage("发送数据失败");
					break;
				}

				this->m_bStepback = false;
			}
			else if(this->m_bPlayerGiveup)
			{													
#ifdef _DEBUG
				this->WriteMsg("服务线程 - 自己下时，自己请求认输，向对方发送(-1,-1)");
#endif
				//执行到此，表示轮到自己下时,自己请求认输，并且对方同意认输
				CPoint ptTmp = CPoint(-1, -1);//因为自己请求认输时，轮到自己下，对方线程处于接收阻塞状态，发送(-1,-1)，进入其线程处于发送数据状态
				memset(cBuf, 0, 128);
				sprintf_s(cBuf, 128, "%d", ptTmp.x * 65535 + ptTmp.y);
				if(SOCKET_ERROR == sockConnect.Send(cBuf, 128))
				{
					AfxMessageBox("Failed to send data!");
					this->m_nResult = RESULT_SEND_FAIL;
					this->SendShowMessage("发送数据失败");
					break;
				}

				if(RGB(0,0,0) == this->GetPlayerChessmanColor())
				{
					this->m_nResult = RESULT_BGIVEUP;
					this->SendShowMessage("黑方认输");
				}
				else if(RGB(255,255,255) == this->GetPlayerChessmanColor())
				{
					this->m_nResult = RESULT_WGIVEUP;
					this->SendShowMessage("白方认输");
				}
				else
				{
					this->m_nResult = RESULT_CONFIG_ERROR;
					this->SendShowMessage("配置错误");
				}

				this->m_bPlayerGiveup = false;
				break;//game over
			}
			else if(this->m_bOpponentGiveup)
			{													
#ifdef _DEBUG
				this->WriteMsg("服务线程 - 自己下时，对手请求认输，向对方发送(-1,-1)");
#endif
				//执行到此，表示轮到自己下时，对手请求认输，并且自己同意认输
				CPoint ptTmp = CPoint(-1, -1);//因为对手请求认输时，轮到自己下，对方线程处于接收阻塞状态，发送(-1,-1)，进入其线程处于发送数据状态
				memset(cBuf, 0, 128);
				sprintf_s(cBuf, 128, "%d", ptTmp.x * 65535 + ptTmp.y);
				if(SOCKET_ERROR == sockConnect.Send(cBuf, 128))
				{	
					AfxMessageBox("Failed to send data!");
					this->m_nResult = RESULT_SEND_FAIL;
					this->SendShowMessage("发送数据失败");
					break;
				}

				if(RGB(0,0,0) == this->GetOpponentChessmanColor())
				{
					this->m_nResult = RESULT_BGIVEUP;
					this->SendShowMessage("黑方认输");
				}
				else if(RGB(255,255,255) == this->GetOpponentChessmanColor())
				{
					this->m_nResult = RESULT_WGIVEUP;
					this->SendShowMessage("白方认输");
				}
				else
				{
					this->m_nResult = RESULT_CONFIG_ERROR;
					this->SendShowMessage("配置错误");
				}

				this->m_bOpponentGiveup = false;
				break;//game over
			}
			else if(this->m_bPlayerEscape)
			{
#ifdef _DEBUG
				this->WriteMsg("服务线程 - 自己下时，自己逃跑，向对方发送(-1,-1)");
#endif
				//执行到此，表示轮到自己下时，自己逃跑
				CPoint ptTmp = CPoint(-1, -1);//轮到自己下时，自己逃跑，对方线程处于接收阻塞状态，发送(-1,-1)，进入其线程处于发送数据状态
				memset(cBuf, 0, 128);

				sprintf_s(cBuf, 128, "%d", ptTmp.x * 65535 + ptTmp.y);
				if(SOCKET_ERROR == sockConnect.Send(cBuf, 128))
				{	
					AfxMessageBox("Failed to send data!");
					this->m_nResult = RESULT_SEND_FAIL;
					this->SendShowMessage("发送数据失败");
					break;
				}

				if(RGB(255,255,255) == this->GetPlayerChessmanColor()) //执白
				{
					this->m_nResult = RESULT_WESCAPE;
					this->SendShowMessage("白方逃跑");
				}
				else if(RGB(0,0,0) == this->GetPlayerChessmanColor())  //执黑
				{
					this->m_nResult = RESULT_BESCAPE;
					this->SendShowMessage("黑方逃跑");
				}
				else
				{
					this->m_nResult = RESULT_CONFIG_ERROR;
					this->SendShowMessage("配置错误");
				}

				this->m_bPlayerEscape = false;
				break;//game over
			}
			else if(this->m_bOpponentEscape)
			{
#ifdef _DEBUG
				this->WriteMsg("服务线程 - 自己下时，对方逃跑，向对方发送(-1,-1)");
#endif
				//执行到此，表示轮到自己下时，对手逃跑
				CPoint ptTmp = CPoint(-1, -1);//轮到自己下时，对方逃跑，对方线程处于接收阻塞状态，发送(-1,-1)，进入其线程处于发送数据状态
				memset(cBuf, 0, 128);

				sprintf_s(cBuf, 128, "%d", ptTmp.x * 65535 + ptTmp.y);
				if(SOCKET_ERROR == sockConnect.Send(cBuf, 128))
				{	
					AfxMessageBox("Failed to send data!");
					this->m_nResult = RESULT_SEND_FAIL;
					this->SendShowMessage("发送数据失败");
					break;
				}

				if(RGB(255,255,255) == this->GetOpponentChessmanColor()) //执白
				{
					this->m_nResult = RESULT_WESCAPE;
					this->SendShowMessage("白方逃跑");
				}
				else if(RGB(0,0,0) == this->GetOpponentChessmanColor())  //执黑
				{
					this->m_nResult = RESULT_BESCAPE;
					this->SendShowMessage("黑方逃跑");
				}
				else
				{
					this->m_nResult = RESULT_CONFIG_ERROR;
					this->SendShowMessage("配置错误");
				}

				this->m_bOpponentEscape = false;
				break;//game over
			}
			else if(0 != this->GetPlayerStepCount())
			{
				CPoint ptTmp = this->m_pPlayer->GetStepPosition(this->GetPlayerStepCount());
				memset(cBuf, 0, 128);
				sprintf_s(cBuf, 128, "%d", ptTmp.x * 65535 + ptTmp.y);
				if(SOCKET_ERROR  == sockConnect.Send(cBuf, 128))//发送自己上次走棋位置
				{
					AfxMessageBox("Failed to send data!");
					this->m_nResult = RESULT_SEND_FAIL;
					this->SendShowMessage("发送数据失败");
					break;
				}
			}

			this->RefreshResult(this->m_pPlayer);
			if(RESULT_NONE != this->m_nResult) break;

			if(RGB(255,255,255) == this->GetOpponentChessmanColor())//对方的颜色是白色
				this->SendShowMessage("白方思考中");
			else
				this->SendShowMessage("黑方思考中");

			memset(cBuf, 0, 128);
			if(0 == sockConnect.Receive(cBuf, 128))//接收对方走棋位置
			{
				AfxMessageBox("Failed to receive data!");
				this->m_nResult = RESULT_RECEIVE_FAIL;
				this->SendShowMessage("接收数据失败");
				break;
			}

			int nTmp = atoi(cBuf);
			CPoint ptGo(nTmp / 65535, nTmp % 65535);
			if(ptGo != CPoint(-1, -1))
				this->OpponentStep(ptGo);
			else //接收到(-1,-1)悔棋，认输，逃跑消除阻塞
			{
#ifdef _DEBUG
				this->WriteMsg("服务线程 - 接收到(-1,-1)");
#endif
				if(this->m_bPlayerGiveup) //对手下，自己请求认输，自己处于阻塞状态，需要对手发送(-1,-1)
				{
					if(RGB(0,0,0) == this->GetPlayerChessmanColor())
					{
						this->m_nResult = RESULT_BGIVEUP;
						this->SendShowMessage("黑方认输");
					}
					else if(RGB(255,255,255) == this->GetPlayerChessmanColor())
					{
						this->m_nResult = RESULT_WGIVEUP;
						this->SendShowMessage("白方认输");
					}
					else
					{
						this->m_nResult = RESULT_CONFIG_ERROR;
						this->SendShowMessage("配置错误");
					}
					break;
				}
				else if(this->m_bOpponentGiveup) //对手下，对手请求认输，自己处于阻塞状态，需要对手发送(-1,-1)
				{
					if(RGB(0,0,0) == this->GetOpponentChessmanColor())
					{
						this->m_nResult = RESULT_BGIVEUP;
						this->SendShowMessage("黑方认输");
					}
					else if(RGB(255,255,255) == this->GetOpponentChessmanColor())
					{
						this->m_nResult = RESULT_WGIVEUP;
						this->SendShowMessage("白方认输");
					}
					else
					{
						this->m_nResult = RESULT_CONFIG_ERROR;
						this->SendShowMessage("配置错误");
					}
					break;
				}
				else if(this->m_bPlayerEscape)//对手下，对手逃跑，自己处于阻塞状态，需要对手发送(-1,-1)
				{
					if(RGB(0,0,0) == this->GetPlayerChessmanColor())
					{
						this->m_nResult = RESULT_BESCAPE;
						this->SendShowMessage("黑方逃跑");
					}
					else if(RGB(255,255,255) == this->GetPlayerChessmanColor())
					{
						this->m_nResult = RESULT_WESCAPE;
						this->SendShowMessage("白方逃跑");
					}
					else
					{
						this->m_nResult = RESULT_CONFIG_ERROR;
						this->SendShowMessage("配置错误");
					}
					break;
				}
				else if(this->m_bOpponentEscape)//对手下，自己逃跑，自己处于阻塞状态，需要对手发送(-1,-1)
				{
					if(RGB(0,0,0) == this->GetOpponentChessmanColor())
					{
						this->m_nResult = RESULT_BESCAPE;
						this->SendShowMessage("黑方逃跑");
					}
					else if(RGB(255,255,255) == this->GetOpponentChessmanColor())
					{
						this->m_nResult = RESULT_WESCAPE;
						this->SendShowMessage("白方逃跑");
					}
					else
					{
						this->m_nResult = RESULT_CONFIG_ERROR;
						this->SendShowMessage("配置错误");
					}
					break;
				}
			}

			this->RefreshResult(this->m_pOpponent);
			if(RESULT_NONE != this->m_nResult) break;

			if(RGB(255,255,255) == this->GetOpponentChessmanColor())//对方的颜色是白色
				this->SendShowMessage("黑方思考中");
			else
				this->SendShowMessage("白方思考中");
		}
		else//该自己走棋
		{
			Sleep(100);
		}	
	}

	sockConnect.Close();
	WSACleanup();
#ifdef _DEBUG
	this->WriteMsg("服务线程 - 结束");
#endif

	this->GameOver();
	return 0;
}

UINT CGobangGame::GameClient(void)
{
	if(!AfxSocketInit())
	{
		AfxMessageBox("Failed to initialize socket!");
		this->m_nResult = RESULT_INIT_FAIL;
		this->SendShowMessage("初始化socket失败");
		return 0;
	}

	CSocket Client;// = new CSocket;
	if(0 ==Client.Create())
	{
		AfxMessageBox("Failed to create socket!");
		this->m_nResult = RESULT_CREATE_FAIL;
		this->SendShowMessage("创建套接字失败");
		WSACleanup();
		return 0;
	}

	if(0 == Client.Connect(this->m_strServerIPAddress, this->m_nPort))
	{
		AfxMessageBox("Failed to connect to server!");
		this->m_nResult = RESULT_CONNECT_FAIL;
		this->SendShowMessage("连接失败");
		Client.Close();
		WSACleanup();
		return 0;
	}

	this->SendShowMessage("已建立连接");

	char cBuf[128];
	memset(cBuf, 0, 128);
	if(0 == Client.Receive(cBuf, 128))//接收对方棋子颜色
	{
		AfxMessageBox("Failed to receive data!");
		this->m_nResult = RESULT_RECEIVE_FAIL;
		this->SendShowMessage("接收数据失败");
		Client.Close();
		WSACleanup();
		return 0;
	}

	COLORREF nTmpColor = (COLORREF)atoi(cBuf);
	nTmpColor = (RGB(255, 255, 255) == nTmpColor ? RGB(0, 0, 0) : RGB(255,255,255));
	this->SetPlayerChessmanColor(nTmpColor);//设置自己棋子颜色

	Sleep(10);
	memset(cBuf, 0, 128);
	sprintf_s(cBuf, 128, "%d", this->GetPlayerChessmanColor());
	if(SOCKET_ERROR == Client.Send(cBuf, 128))//发送自己的棋子颜色，以便进行验证
	{
		AfxMessageBox("Failed to send data!");
		this->m_nResult = RESULT_SEND_FAIL;
		this->SendShowMessage("发送数据失败");
		Client.Close();
		WSACleanup();
		return 0;
	}

	memset(cBuf, 0, 128);
	if(0 == Client.Receive(cBuf, 128))
	{
		AfxMessageBox("Failed to receive data!");
		this->m_nResult = RESULT_RECEIVE_FAIL;
		this->SendShowMessage("接收数据失败");
		Client.Close();
		WSACleanup();
		return 0;
	}

	if(0 != strcmp(cBuf, "START"))
	{
		AfxMessageBox("Failed to receive start command!");
		this->m_nResult = RESULT_CONFIG_ERROR;
		this->SendShowMessage("配置错误");
		Client.Close();
		WSACleanup();
		return 0;
	}

	if(SOCKET_ERROR == Client.Send("START", sizeof("START")))
	{
		AfxMessageBox("Failed to send data!");
		this->m_nResult = RESULT_SEND_FAIL;
		this->SendShowMessage("发送数据失败");
		Client.Close();
		WSACleanup();
		return 0;
	}

	this->SendShowMessage("开始游戏");

	this->m_bStart = true;
	if(NULL == m_pThreadListen) m_pThreadListen = AfxBeginThread(ThreadListen, (LPVOID)this);
	AfxBeginThread(this->ThreadHightlight, (LPVOID)this);

	if(RGB(255,255,255) == this->GetPlayerChessmanColor())//自己的颜色为白色
	{
		this->SetPlayerTurn();//自己下
		this->SendShowMessage("白方思考中");
	}
	else//自己的颜色为黑色
	{
		this->SetOpponentTurn();//对方下
		this->SendShowMessage("黑方思考中");
	}
	while(RESULT_NONE == this->m_nResult)
	{
		if(this->IsOpponentTurn())//对手下
		{
			if(this->m_bStepback)
			{	
#ifdef _DEBUG
				this->WriteMsg("客户线程 - 轮到自己下时，对手请求悔棋，向对手发送(-1,-1)");
#endif
				//执行到此，表示轮到自己下时，对手请求悔棋。悔棋后又轮到对手下
				CPoint ptTmp = CPoint(-1, -1);//因为对手悔棋时，轮到自己下，对方线程处在接收阻塞状态，发送(-1,-1)，进入其线程处于发送数据状态
				memset(cBuf, 0, 128);
				sprintf_s(cBuf, 128, "%d", ptTmp.x * 65535 + ptTmp.y);
				if(SOCKET_ERROR == Client.Send(cBuf, 128))
				{
					AfxMessageBox("Failed to send data!");
					this->m_nResult = RESULT_SEND_FAIL;
					this->SendShowMessage("发送数据失败");
					break;
				}

				this->m_bStepback = false;
			}
			else if(this->m_bPlayerGiveup)
			{													
#ifdef _DEBUG
				this->WriteMsg("客户线程 - 自己下时,自己请求认输，向对手发送(-1,-1)");
#endif
				//执行到此，表示轮到自己下时,自己请求认输，并且对方同意认输
				CPoint ptTmp = CPoint(-1, -1);//因为自己请求认输时，轮到自己下，对方线程处于接收阻塞状态，发送(-1,-1)，进入其线程处于发送数据状态
				memset(cBuf, 0, 128);
				sprintf_s(cBuf, 128, "%d", ptTmp.x * 65535 + ptTmp.y);
				if(SOCKET_ERROR == Client.Send(cBuf, 128))
				{
					AfxMessageBox("Failed to send data!");
					this->m_nResult = RESULT_SEND_FAIL;
					this->SendShowMessage("发送数据失败");
					break;
				}

				if(RGB(0,0,0) == this->GetPlayerChessmanColor())
				{
					this->m_nResult = RESULT_BGIVEUP;
					this->SendShowMessage("黑方认输");
				}
				else if(RGB(255,255,255) == this->GetPlayerChessmanColor())
				{
					this->m_nResult = RESULT_WGIVEUP;
					this->SendShowMessage("白方认输");
				}
				else
				{
					this->m_nResult = RESULT_CONFIG_ERROR;
					this->SendShowMessage("配置错误");
				}

				this->m_bPlayerGiveup = false;
				break;//game over
			}
			else if(this->m_bOpponentGiveup)
			{													
#ifdef _DEBUG
				this->WriteMsg("客户线程 - 自己下时，对手请求认输，向对手发送(-1,-1)");
#endif
				//执行到此，表示轮到自己下时,对手请求认输，并且自己同意认输
				CPoint ptTmp = CPoint(-1, -1);//因为对手请求认输时，轮到自己下，对方线程处于接收阻塞状态，发送(-1,-1)，进入其线程处于发送数据状态
				memset(cBuf, 0, 128);
				sprintf_s(cBuf, 128, "%d", ptTmp.x * 65535 + ptTmp.y);
				if(SOCKET_ERROR == Client.Send(cBuf, 128))
				{
					AfxMessageBox("Failed to send data!");
					this->m_nResult = RESULT_SEND_FAIL;
					this->SendShowMessage("发送数据失败");
					break;
				}

				if(RGB(0,0,0) == this->GetOpponentChessmanColor())
				{
					this->m_nResult = RESULT_BGIVEUP;
					this->SendShowMessage("黑方认输");
				}
				else if(RGB(255,255,255) == this->GetOpponentChessmanColor())
				{
					this->m_nResult = RESULT_WGIVEUP;
					this->SendShowMessage("白方认输");
				}
				else
				{
					this->m_nResult = RESULT_CONFIG_ERROR;
					this->SendShowMessage("配置错误");
				}

				this->m_bOpponentGiveup = false;
				break;//game over
			}
			else if(this->m_bPlayerEscape)
			{
#ifdef _DEBUG
				this->WriteMsg("客户线程 - 自己下时，自己逃跑，向对方发送(-1,-1)");
#endif
				//执行到此，表示轮到自己下时，自己逃跑
				CPoint ptTmp = CPoint(-1, -1);//轮到自己下时，自己逃跑，对方线程处于接收阻塞状态，发送(-1,-1)，进入其线程处于发送数据状态
				memset(cBuf, 0, 128);

				sprintf_s(cBuf, 128, "%d", ptTmp.x * 65535 + ptTmp.y);
				if(SOCKET_ERROR == Client.Send(cBuf, 128))
				{	
					AfxMessageBox("Failed to send data!");
					this->m_nResult = RESULT_SEND_FAIL;
					this->SendShowMessage("发送数据失败");
					break;
				}

				if(RGB(255,255,255) == this->GetPlayerChessmanColor()) //执白
				{
					this->m_nResult = RESULT_WESCAPE;
					this->SendShowMessage("白方逃跑");
				}
				else if(RGB(0,0,0) == this->GetPlayerChessmanColor())  //执黑
				{
					this->m_nResult = RESULT_BESCAPE;
					this->SendShowMessage("黑方逃跑");
				}
				else
				{
					this->m_nResult = RESULT_CONFIG_ERROR;
					this->SendShowMessage("配置错误");
				}

				this->m_bPlayerEscape = false;
				break;//game over
			}
			else if(this->m_bOpponentEscape)
			{
#ifdef _DEBUG
				this->WriteMsg("服务线程 - 自己下时，对方逃跑，向对方发送(-1,-1)");
#endif
				//执行到此，表示轮到自己下时，对手逃跑
				CPoint ptTmp = CPoint(-1, -1);//轮到自己下时，对方逃跑，对方线程处于接收阻塞状态，发送(-1,-1)，进入其线程处于发送数据状态
				memset(cBuf, 0, 128);

				sprintf_s(cBuf, 128, "%d", ptTmp.x * 65535 + ptTmp.y);
				if(SOCKET_ERROR == Client.Send(cBuf, 128))
				{	
					AfxMessageBox("Failed to send data!");
					this->m_nResult = RESULT_SEND_FAIL;
					this->SendShowMessage("发送数据失败");
					break;
				}

				if(RGB(255,255,255) == this->GetOpponentChessmanColor()) //执白
				{
					this->m_nResult = RESULT_WESCAPE;
					this->SendShowMessage("白方逃跑");
				}
				else if(RGB(0,0,0) == this->GetOpponentChessmanColor())  //执黑
				{
					this->m_nResult = RESULT_BESCAPE;
					this->SendShowMessage("黑方逃跑");
				}
				else
				{
					this->m_nResult = RESULT_CONFIG_ERROR;
					this->SendShowMessage("配置错误");
				}

				this->m_bOpponentEscape = false;
				break;//game over
			}
			else if(0 != this->GetPlayerStepCount())
			{
				CPoint ptTmp = this->m_pPlayer->GetStepPosition(this->GetPlayerStepCount());
				memset(cBuf, 0, 128);
				sprintf_s(cBuf, 128, "%d", ptTmp.x * 65535 + ptTmp.y);//发送自己上一次走棋位置
				if(SOCKET_ERROR == Client.Send(cBuf, 128))
				{
					AfxMessageBox("Failed to send data!");
					this->m_nResult = RESULT_SEND_FAIL;
					//this->SendShowMessage("发送数据失败");
					break;
				}
			}

			this->RefreshResult(this->m_pPlayer);
			if(RESULT_NONE != this->m_nResult) break;

			if(RGB(255,255,255) == this->GetOpponentChessmanColor())//对方的颜色是白色
				this->SendShowMessage("白方思考中");
			else
				this->SendShowMessage("黑方思考中");

			memset(cBuf, 0, 128);
			if(0 == Client.Receive(cBuf, 128))//接收对方走棋位置
			{
				AfxMessageBox("Failed to receive data!");
				this->m_nResult = RESULT_RECEIVE_FAIL;
				this->SendShowMessage("发送数据失败");
				break;
			}

			int nTmp = atoi(cBuf);
			CPoint ptGo(nTmp / 65535, nTmp % 65535);
			if(ptGo != CPoint(-1, -1))
				this->OpponentStep(ptGo);
			else //接收到(-1,-1)，悔棋，认输，消除阻塞
			{
#ifdef _DEBUG
				this->WriteMsg("客户线程 - 接收到(-1,-1)");
#endif
				if(this->m_bPlayerGiveup) //对手下，自己请求认输，自己处于阻塞状态，需要对手发送(-1,-1)
				{
					if(RGB(0,0,0) == this->GetPlayerChessmanColor())
					{
						this->m_nResult = RESULT_BGIVEUP;
						this->SendShowMessage("黑方认输");
					}
					else if(RGB(255,255,255) == this->GetPlayerChessmanColor())
					{
						this->m_nResult = RESULT_WGIVEUP;
						this->SendShowMessage("白方认输");
					}
					else
					{
						this->m_nResult = RESULT_CONFIG_ERROR;
						this->SendShowMessage("配置错误");
					}
					break;
				}
				else if(this->m_bOpponentGiveup) //对手下，对手请求认输，自己处于阻塞状态，需要对手发送(-1,-1)
				{
					if(RGB(0,0,0) == this->GetOpponentChessmanColor())
					{
						this->m_nResult = RESULT_BGIVEUP;
						this->SendShowMessage("黑方认输");
					}
					else if(RGB(255,255,255) == this->GetOpponentChessmanColor())
					{
						this->m_nResult = RESULT_WGIVEUP;
						this->SendShowMessage("白方认输");
					}
					else
					{
						this->m_nResult = RESULT_CONFIG_ERROR;
						this->SendShowMessage("配置错误");
					}
					break;
				}
				else if(this->m_bPlayerEscape)//对手下，自己逃跑，自己处于阻塞状态，需要对手发送(-1,-1)
				{
					if(RGB(0,0,0) == this->GetPlayerChessmanColor())
					{
						this->m_nResult = RESULT_BESCAPE;
						this->SendShowMessage("黑方逃跑");
					}
					else if(RGB(255,255,255) == this->GetPlayerChessmanColor())
					{
						this->m_nResult = RESULT_WESCAPE;
						this->SendShowMessage("白方逃跑");
					}
					else
					{
						this->m_nResult = RESULT_CONFIG_ERROR;
						this->SendShowMessage("配置错误");
					}
					break;
				}
				else if(this->m_bOpponentEscape)//对手下，对手逃跑，自己处于阻塞状态，需要对手发送(-1,-1)
				{
					if(RGB(0,0,0) == this->GetOpponentChessmanColor())
					{
						this->m_nResult = RESULT_BESCAPE;
						this->SendShowMessage("黑方逃跑");
					}
					else if(RGB(255,255,255) == this->GetOpponentChessmanColor())
					{
						this->m_nResult = RESULT_WESCAPE;
						this->SendShowMessage("白方逃跑");
					}
					else
					{
						this->m_nResult = RESULT_CONFIG_ERROR;
						this->SendShowMessage("配置错误");
					}
					break;
				}
			}

			this->RefreshResult(this->m_pOpponent);
			if(RESULT_NONE != this->m_nResult) break;

			if(RGB(255,255,255) == this->GetOpponentChessmanColor())//对方的颜色是白色
				this->SendShowMessage("黑方思考中");
			else
				this->SendShowMessage("白方思考中");
		}	
		else//自己走棋
		{		
			Sleep(100);
		}
	}

	Client.Close();
	WSACleanup();

#ifdef _DEBUG
	this->WriteMsg("客户线程 - 结束");
#endif

	this->GameOver();
	return 0;
}

UINT CGobangGame::GameListen(void)
{
	if(!AfxSocketInit())
	{
		AfxMessageBox("GameListen() - Failed to initialize socket!");
		this->m_nResult = RESULT_INIT_FAIL;
		this->SendShowMessage("初始化socket失败");
		return 0;
	}

	CSocket Listen;
	int nPort;
	CString strLocalIP;

	if(!GetLocalIP(NULL, &strLocalIP))
	{
		AfxMessageBox("GameListen() - Failed to get local IP address!");
		this->m_nResult = RESULT_INIT_FAIL;
		this->SendShowMessage("得到IP失败");
		WSACleanup();
		return 0;
	}

	if(MODE_SERVER == this->m_nMode)
		nPort = this->m_nPort + 1;
	else if(MODE_CLIENT== this->m_nMode) 
		nPort = this->m_nPort - 1;
	else 
	{
		AfxMessageBox("GameListen() - Failed to config!");
		this->m_nResult = RESULT_CONFIG_ERROR;
		this->SendShowMessage("配置错误");
		WSACleanup();
		return 0;
	}

	if(0 == Listen.Create(nPort, SOCK_STREAM, strLocalIP))
	{
		AfxMessageBox("GameListen() - Failed to create socket!");
		this->m_nResult = RESULT_CREATE_FAIL;
		this->SendShowMessage("创建socket失败");
		WSACleanup();
		return 0;
	}

	if(0 == Listen.Listen())
	{
		AfxMessageBox("GameListen() - Failed to listen!");
		this->m_nResult = RESULT_LISTEN_FAIL;
		this->SendShowMessage("监听失败");
		Listen.Close();
		WSACleanup();
		return 0;
	}

	while(1)
	{
	#ifdef _DEBUG
		this->WriteMsg("监听线程 - 开始监听");
	#endif

		//if(0 == Listen.Listen())
		//{
		//	AfxMessageBox("GameListen() - Failed to listen!");
		//	this->m_nResult = RESULT_LISTEN_FAIL;
		//	break;
		//}

		CSocket sockConnect;
		if(0 == Listen.Accept(sockConnect))
		{
			AfxMessageBox("GameListen() - Failed to accept!");
			this->m_nResult = RESULT_CONNECT_FAIL;
			this->SendShowMessage("接受连接失败");
			break;
		}

	#ifdef _DEBUG
		this->WriteMsg("监听线程 - 有一个连接");
	#endif

		this->GameReply(&sockConnect);

	//	char cBuf[128];
	//	memset(cBuf, 0, 128);
	//	if(0 == sockConnect.Receive(cBuf, 128)) //接受主要请求信息
	//	{
	//		AfxMessageBox("GameListen() - Failed to receive data!");
	//		this->m_nResult = RESULT_RECEIVE_FAIL;
	//		sockConnect.Close();
	//		WSACleanup();
	//		return 0;
	//	}	

	//	if(0 == strcmp(cBuf, "STEPBACK"))
	//	{
	//#ifdef _DEBUG
	//		this->WriteMsg("监听线程 - 收到对手悔棋请求");
	//#endif
	//		if(IDOK == AfxMessageBox("对方请求悔棋，同意吗？", MB_ICONQUESTION | MB_OKCANCEL))//同意悔棋
	//		{
	//			if(0 == sockConnect.Send("YES", (int)strlen("YES")))
	//			{
	//				AfxMessageBox("GameListen() - Failed to send data!");
	//				this->m_nResult = RESULT_SEND_FAIL;
	//				sockConnect.Close();
	//				WSACleanup();
	//				return 0;
	//			}
	//#ifdef _DEBUG
	//			this->WriteMsg("监听线程 - 同意对手悔棋");
	//#endif
	//			if(this->IsOpponentTurn())//轮到对手下，对手请求悔棋
	//			{
	//#ifdef _DEBUG
	//				this->WriteMsg("监听线程 - 确认轮到对手下，对手请求悔棋，自己线程处于接收状态，不需要对手发送(-1,-1)");
	//#endif
	//				this->m_pOpponent->StepBack();
	//				this->m_pPlayer->StepBack();
	//			}
	//			else if(this->IsPlayerTurn())//轮到自己下，对手请求悔棋
	//			{
	//#ifdef _DEBUG
	//				this->WriteMsg("监听线程 - 确认轮到自己下，对手请求悔棋，对手线程处于接收状态，需要自己发送(-1,-1)");
	//#endif
	//				this->m_pOpponent->StepBack(); 

	//				this->m_bStepback = true;//悔棋标志true，因为自己下时，对手线程处于接收数据阻塞状态
	//				this->SetOpponentTurn();//设置为对手下，向对手发送(-1,-1)，从而使对手线程发送数据状态
	//			}

	//			if(RGB(255,255,255) == this->GetOpponentChessmanColor())
	//				this->SendShowMessage("白方思考中");
	//			else
	//				this->SendShowMessage("黑方思考中");

	//			this->m_pMainWnd->Invalidate();
	//			this->m_pMainWnd->UpdateWindow();
	//		}
	//		else
	//		{
	//#ifdef _DEBUG
	//			this->WriteMsg("监听线程 - 不同意对手悔棋");
	//#endif
	//			if(0 == sockConnect.Send("NO", (int)strlen("NO")))
	//			{
	//				AfxMessageBox("GameListen() - Failed to send data!");
	//				this->m_nResult = RESULT_SEND_FAIL;
	//				sockConnect.Close();
	//				WSACleanup();
	//				return 0;
	//			}
	//		}
	//	}
	//	else if(0 == strcmp(cBuf, "GIVEUP"))
	//	{
	//#ifdef _DEBUG
	//		this->WriteMsg("监听线程 - 收到对手认输请求");
	//#endif
	//		if(IDOK == AfxMessageBox("对方请求认输，同意吗？", MB_ICONQUESTION | MB_OKCANCEL))//同意
	//		{
	//			if(0 == sockConnect.Send("YES", (int)strlen("YES")))
	//			{
	//				AfxMessageBox("GameListen() - Failed to send data!");
	//				this->m_nResult = RESULT_SEND_FAIL;
	//				sockConnect.Close();
	//				WSACleanup();
	//				return 0;
	//			}

	//#ifdef _DEBUG
	//			this->WriteMsg("监听线程 - 同意对手认输");
	//#endif
	//			this->m_bOpponentGiveup = true;//认输标志true，因为自己下时，对手线程处于接收数据阻塞状态
	//			if(this->IsPlayerTurn())//轮到自己下，对手请求认输
	//			{
	//#ifdef _DEBUG
	//				this->WriteMsg("监听线程 - 确认轮到自己走棋，对方请求悔棋，对方线程处于接收阻塞状态，需要自己发送(-1,-1)");
	//#endif
	//				this->SetOpponentTurn();//设置为对手下，向对手发送(-1,-1)，从而使对手线程发送数据状态
	//			}
	//		}
	//		else
	//		{
	//#ifdef _DEBUG
	//			this->WriteMsg("监听线程 - 不同意对手悔棋");
	//#endif
	//			if(0 == sockConnect.Send("NO", (int)strlen("NO")))
	//			{
	//				AfxMessageBox("GameListen() - Failed to send data!");
	//				this->m_nResult = RESULT_SEND_FAIL;
	//				sockConnect.Close();
	//				WSACleanup();
	//				return 0;
	//			}
	//		}
	//	}
	//	else if(0 == strcmp(cBuf, "QUIT"))
	//	{
	//		char cBuf1[128];
	//		memset(cBuf1, 0, 128);
	//		if(0 == sockConnect.Receive(cBuf1, 128))  //接受副请求信息
	//		{
	//			AfxMessageBox("GameListen() - Failed to receive data!");
	//			this->m_nResult = RESULT_RECEIVE_FAIL;
	//			sockConnect.Close();
	//			WSACleanup();
	//			return 0;
	//		}

	//		CString strMsg = "";
	//		strMsg.Format("对方请求退出游戏！\n理由：%s\n同意吗？", cBuf1);
	//		if(IDOK == AfxMessageBox(strMsg, MB_ICONQUESTION | MB_OKCANCEL))//同意
	//		{
	//			if(0 == sockConnect.Send("YES", (int)strlen("YES")))
	//			{
	//				AfxMessageBox("GameListen() - Failed to send data!");
	//				this->m_nResult = RESULT_SEND_FAIL;
	//				sockConnect.Close();
	//				WSACleanup();
	//				return 0;
	//			}

	//			this->m_bOpponentGiveup = true;//认输标志true，因为自己下时，对手线程处于接收数据阻塞状态
	//			if(this->IsPlayerTurn())//轮到自己下，对手请求认输
	//			{
	//				this->SetOpponentTurn();//设置为对手下，向对手发送(-1,-1)，从而使对手线程发送数据状态
	//			}
	//		}
	//		else
	//		{
	//			if(0 == sockConnect.Send("NO", (int)strlen("NO")))
	//			{
	//				AfxMessageBox("GameListen() - Failed to send data!");
	//				this->m_nResult = RESULT_SEND_FAIL;
	//				sockConnect.Close();
	//				WSACleanup();
	//				return 0;
	//			}
	//		}
	//	}
	//	else if(0 == strcmp(cBuf, "ESCAPE"))
	//	{
	//#ifdef _DEBUG
	//		this->WriteMsg("监听线程 - 收到对方逃跑信号");
	//#endif
	//		this->m_bOpponentEscape = true;
	//		if(this->IsPlayerTurn())
	//		{
	//#ifdef _DEBUG
	//			this->WriteMsg("监听线程 - 确认轮到自己走棋，对方逃跑，对方线程处于接收阻塞状态，需要自己发送(-1,-1)");
	//#endif
	//			this->SetOpponentTurn();
	//		}	
	//		AfxMessageBox("对方逃跑！");
	//	}
	//	else
	//	{
	//		AfxMessageBox(cBuf);
	//	}

	//	sockConnect.Close();
	}

	Listen.Close();
	WSACleanup();

	return 0;
}

UINT CGobangGame::GameStepback(void)
{
#ifdef _DEBUG
	this->WriteMsg("悔棋线程 - 发送悔棋请求");
#endif

	this->m_bWaitting = true;

	if(this->Request("STEPBACK"))
	{
#ifdef _DEBUG
		this->WriteMsg("悔棋线程 - 对方同意自己悔棋");
#endif
		this->SendShowMessage("对方同意悔棋");
		AfxMessageBox("对方同意悔棋");

		if(this->IsOpponentTurn())
		{
#ifdef _DEBUG
			this->WriteMsg("悔棋线程 - 确认轮到对手下棋，自己请求悔棋，自己的线程处于接收阻塞状态，需要对手发送(-1,-1)");
#endif
			this->m_pPlayer->StepBack();
			this->SetPlayerTurn();
		}
		else if(this->IsPlayerTurn())
		{
#ifdef _DEBUG
			this->WriteMsg("悔棋线程 - 确认轮到自己下棋，自己请求悔棋，对手的线程处于接收阻塞状态，不需要自己对手发送(-1,-1)");
#endif
			this->m_pPlayer->StepBack();
			this->m_pOpponent->StepBack();
		}

		if(RGB(255,255,255) == this->GetPlayerChessmanColor())
			this->SendShowMessage("白方思考中");
		else
			this->SendShowMessage("黑方思考中");

		this->m_pMainWnd->Invalidate();
		this->m_pMainWnd->UpdateWindow();
	}
	else
	{
#ifdef _DEBUG
		this->WriteMsg("悔棋线程 - 对方不同意悔棋");
#endif
		this->SendShowMessage("对方不同意悔棋");
		AfxMessageBox("对方不同意悔棋！");
	}

	this->m_bWaitting = false;
	return 0;
}

UINT CGobangGame::GameGiveup(void)
{

#ifdef _DEBUG
	this->WriteMsg("认输线程 - 发送认输请求");
#endif

	this->m_bWaitting = true;

	if(this->Request("GIVEUP")) //对方同意认输
	{
#ifdef _DEBUG
		this->WriteMsg("认输线程 - 对方同意自己认输");
#endif
		this->SendShowMessage("对方同意认输");
		AfxMessageBox("对方同意认输");

		this->m_bPlayerGiveup = true; //认输标记true
		if(this->IsPlayerTurn()) //轮到自己下，自己请求认输，此时对方线程处于接收阻塞状态
		{
#ifdef _DEBUG
			this->WriteMsg("认输线程 - 确认轮到自己走棋，自己请求认输，对手线程处于接收阻塞状态，需要自己发送(-1,-1)");
#endif
			this->SetOpponentTurn();
		}
	}
	else
	{
#ifdef _DEBUG
		this->WriteMsg("认输线程 - 对手不同意认输");
#endif
		this->SendShowMessage("对方不同意认输");
		AfxMessageBox("对方不同意认输！");
	}

	this->m_bWaitting = false;
	return 0;
}

UINT CGobangGame::GameQuit(void)
{
	this->m_bWaitting = true;

	if(this->Request("QUIT"))
	{
		this->m_bPlayerGiveup = true; //认输标记true

		if(this->IsPlayerTurn()) //轮到自己下，自己请求退出，此时对方线程处于接收阻塞状态
		{
			this->SetOpponentTurn();
		}

		Sleep(100); 

		::PostMessage(this->m_pMainWnd->m_hWnd, WM_QUIT, 0, 0);
	}
	else
	{
		this->SendShowMessage("对方不同意你退出游戏");
		AfxMessageBox("对方不同意退出游戏！");	
	}
	
	this->m_bWaitting = false;
	return 0;
}

UINT CGobangGame::GameReply(CSocket * pSocket)
{
	char cBuf[128];
	memset(cBuf, 0, 128);
	if(0 == pSocket->Receive(cBuf, 128)) //接受主要请求信息
	{
		AfxMessageBox("GameListen() - Failed to receive data!");
		this->m_nResult = RESULT_RECEIVE_FAIL;
		this->SendShowMessage("接收数据失败");
		pSocket->Close();
		WSACleanup();
		return 0;
	}	

	if(0 == strcmp(cBuf, "STEPBACK"))
	{
#ifdef _DEBUG
		this->WriteMsg("监听线程 - 收到对手悔棋请求");
#endif
		if(IDOK == AfxMessageBox("对方请求悔棋，同意吗？", MB_ICONQUESTION | MB_OKCANCEL))//同意悔棋
		{
			if(0 == pSocket->Send("YES", (int)strlen("YES")))
			{
				AfxMessageBox("GameListen() - Failed to send data!");
				this->m_nResult = RESULT_SEND_FAIL;
				this->SendShowMessage("发送数据失败");
				pSocket->Close();
				WSACleanup();
				return 0;
			}
#ifdef _DEBUG
			this->WriteMsg("监听线程 - 同意对手悔棋");
#endif
			if(this->IsOpponentTurn())//轮到对手下，对手请求悔棋
			{
#ifdef _DEBUG
				this->WriteMsg("监听线程 - 确认轮到对手下，对手请求悔棋，自己线程处于接收状态，不需要对手发送(-1,-1)");
#endif
				this->m_pOpponent->StepBack();
				this->m_pPlayer->StepBack();
			}
			else if(this->IsPlayerTurn())//轮到自己下，对手请求悔棋
			{
#ifdef _DEBUG
				this->WriteMsg("监听线程 - 确认轮到自己下，对手请求悔棋，对手线程处于接收状态，需要自己发送(-1,-1)");
#endif
				this->m_pOpponent->StepBack(); 

				this->m_bStepback = true;//悔棋标志true，因为自己下时，对手线程处于接收数据阻塞状态
				this->SetOpponentTurn();//设置为对手下，向对手发送(-1,-1)，从而使对手线程发送数据状态
			}

			if(RGB(255,255,255) == this->GetOpponentChessmanColor())
				this->SendShowMessage("白方思考中");
			else
				this->SendShowMessage("黑方思考中");

			this->m_pMainWnd->Invalidate();
			this->m_pMainWnd->UpdateWindow();
		}
		else
		{
#ifdef _DEBUG
			this->WriteMsg("监听线程 - 不同意对手悔棋");
#endif
			if(0 == pSocket->Send("NO", (int)strlen("NO")))
			{
				AfxMessageBox("GameListen() - Failed to send data!");
				this->m_nResult = RESULT_SEND_FAIL;
				this->SendShowMessage("发送数据失败");
				pSocket->Close();
				WSACleanup();
				return 0;
			}
		}
	}
	else if(0 == strcmp(cBuf, "GIVEUP"))
	{
#ifdef _DEBUG
		this->WriteMsg("监听线程 - 收到对手认输请求");
#endif
		if(IDOK == AfxMessageBox("对方请求认输，同意吗？", MB_ICONQUESTION | MB_OKCANCEL))//同意
		{
			if(0 == pSocket->Send("YES", (int)strlen("YES")))
			{
				AfxMessageBox("GameListen() - Failed to send data!");
				this->m_nResult = RESULT_SEND_FAIL;
				this->SendShowMessage("发送数据失败");
				pSocket->Close();
				WSACleanup();
				return 0;
			}

#ifdef _DEBUG
			this->WriteMsg("监听线程 - 同意对手认输");
#endif
			this->m_bOpponentGiveup = true;//认输标志true，因为自己下时，对手线程处于接收数据阻塞状态
			if(this->IsPlayerTurn())//轮到自己下，对手请求认输
			{
#ifdef _DEBUG
				this->WriteMsg("监听线程 - 确认轮到自己走棋，对方请求悔棋，对方线程处于接收阻塞状态，需要自己发送(-1,-1)");
#endif
				this->SetOpponentTurn();//设置为对手下，向对手发送(-1,-1)，从而使对手线程发送数据状态
			}
		}
		else
		{
#ifdef _DEBUG
			this->WriteMsg("监听线程 - 不同意对手悔棋");
#endif
			if(0 == pSocket->Send("NO", (int)strlen("NO")))
			{
				AfxMessageBox("GameListen() - Failed to send data!");
				this->m_nResult = RESULT_SEND_FAIL;
				this->SendShowMessage("发送数据失败");
				pSocket->Close();
				WSACleanup();
				return 0;
			}
		}
	}
	else if(0 == strcmp(cBuf, "QUIT"))
	{
		char cBuf1[128];
		memset(cBuf1, 0, 128);
		if(0 == pSocket->Receive(cBuf1, 128))  //接受副请求信息
		{
			AfxMessageBox("GameListen() - Failed to receive data!");
			this->m_nResult = RESULT_RECEIVE_FAIL;
			this->SendShowMessage("接收数据失败");
			pSocket->Close();
			WSACleanup();
			return 0;
		}

		CString strMsg = "";
		strMsg.Format("对方请求退出游戏！\n理由：%s\n同意吗？", cBuf1);
		if(IDOK == AfxMessageBox(strMsg, MB_ICONQUESTION | MB_OKCANCEL))//同意
		{
			if(0 == pSocket->Send("YES", (int)strlen("YES")))
			{
				AfxMessageBox("GameListen() - Failed to send data!");
				this->m_nResult = RESULT_SEND_FAIL;
				this->SendShowMessage("发送数据失败");
				pSocket->Close();
				WSACleanup();
				return 0;
			}

			this->m_bOpponentGiveup = true;//认输标志true，因为自己下时，对手线程处于接收数据阻塞状态
			if(this->IsPlayerTurn())//轮到自己下，对手请求认输
			{
				this->SetOpponentTurn();//设置为对手下，向对手发送(-1,-1)，从而使对手线程发送数据状态
			}
		}
		else
		{
			if(0 == pSocket->Send("NO", (int)strlen("NO")))
			{
				AfxMessageBox("GameListen() - Failed to send data!");
				this->m_nResult = RESULT_SEND_FAIL;
				this->SendShowMessage("发送数据失败");
				pSocket->Close();
				WSACleanup();
				return 0;
			}
		}
	}
	else if(0 == strcmp(cBuf, "ESCAPE"))
	{
#ifdef _DEBUG
		this->WriteMsg("监听线程 - 收到对方逃跑信号");
#endif

		if(0 == pSocket->Send("YES", (int)strlen("YES")))
		{
			AfxMessageBox("GameListen() - Failed to send data!");
			this->m_nResult = RESULT_SEND_FAIL;
			this->SendShowMessage("发送数据失败");
			pSocket->Close();
			WSACleanup();
			return 0;
		}

		this->m_bOpponentEscape = true;
		if(this->IsPlayerTurn())
		{
#ifdef _DEBUG
			this->WriteMsg("监听线程 - 确认轮到自己走棋，对方逃跑，对方线程处于接收阻塞状态，需要自己发送(-1,-1)");
#endif
			this->SetOpponentTurn();
		}	
		//AfxMessageBox("对方逃跑！");
	}
	else
	{
		AfxMessageBox(cBuf);
	}

	pSocket->Close();
	WSACleanup();

	return 0;
}

UINT CGobangGame::GameEscape(void)
{
#ifdef _DEBUG
	this->WriteMsg("逃跑线程 - 发送逃跑信号");
#endif

	this->m_bWaitting = true;

	this->Request("ESCAPE");

	this->m_bPlayerEscape = true;
	if(this->IsPlayerTurn())
	{
#ifdef _DEBUG
		this->WriteMsg("逃跑线程 - 确认轮到自己走棋，自己逃跑，对手线程处于接收阻塞状态，需要自己发送(-1,-1)");
#endif
		this->SetOpponentTurn();
	}
	else if(this->IsOpponentTurn())
	{
#ifdef _DEBUG
		this->WriteMsg("逃跑线程 - 确认轮到对方走棋，自己逃跑，自己线程处于接收阻塞状态，需要对方发送(-1,-1)");
#endif
	}

	this->m_bWaitting = false;
	return 0;
}

UINT CGobangGame::ThreadSingle(LPVOID pParam)
{
	CGobangGame * pGobangGame = (CGobangGame *)pParam;
	return pGobangGame->GameSingle();
}

UINT CGobangGame::ThreadServer(LPVOID pParam)
{
	CGobangGame * pGobangGame = (CGobangGame *)pParam;
	return pGobangGame->GameServer();
}

UINT CGobangGame::ThreadClient(LPVOID pParam)
{
	CGobangGame * pGobangGame = (CGobangGame *)pParam;
	return pGobangGame->GameClient();
}

UINT CGobangGame::ThreadListen(LPVOID pParam)
{
	CGobangGame *pGobangGame = (CGobangGame*)pParam;
	return pGobangGame->GameListen();	
}

UINT CGobangGame::ThreadStepBack(LPVOID pParam)
{
	CGobangGame *pGobangGame = (CGobangGame*)pParam;
	return pGobangGame->GameStepback();
}

UINT CGobangGame::ThreadGiveup(LPVOID pParam)
{
	CGobangGame *pGobangGame = (CGobangGame*)pParam;
	return pGobangGame->GameGiveup();	
}

UINT CGobangGame::ThreadQuit(LPVOID pParam)
{
	CGobangGame *pGobangGame = (CGobangGame*)pParam;
	return pGobangGame->GameQuit();
}

UINT CGobangGame::ThreadEscape(LPVOID pParam)
{
	CGobangGame *pGobangGame = (CGobangGame*)pParam;
	return pGobangGame->GameEscape();	
}

UINT CGobangGame::ThreadHightlight(LPVOID pParam)
{
	CGobangGame *pGobangGame = (CGobangGame*)pParam;
	return pGobangGame->GameHighlightChessman();
}

NUMBER_TYPE CGobangGame::GetPointNT(const CPlayer * pPlayer, const CPoint & Point, AZIMUTH_ANGLE nAzimuth) const
{
	NUMBER_TYPE nType = NT_UNKNOWN;
	NUMBER_TYPE nTypeTmp = NT_UNKNOWN;

	const CPlayer * pOpponent = NULL;
	CRect rectBoard = this->m_pChessboard->GetChessboardSize();
	
	CPoint pt1, pt2, pt3, pt4;

	if(this->m_pPlayer == pPlayer)
	{pOpponent = this->m_pOpponent;}
	else if(this->m_pOpponent == pPlayer)
	{pOpponent = this->m_pPlayer;}
	else
	{return nType;}

	pt1 = Point;
	this->GetNextPoint(nAzimuth, pt1);
	pt2 = pt1;
	this->GetNextPoint(nAzimuth, pt2);
	pt3 = pt2;
	this->GetNextPoint(nAzimuth, pt3);
	pt4 = pt3;
	this->GetNextPoint(nAzimuth, pt4);

	if(pOpponent->IsPositionStepped(pt1)) //pt1被对手占，死1
	{nType = NT_DEAD_1;} 
	else if(pPlayer->IsPositionStepped(pt1))//pt1被自己占
	{
		if(pOpponent->IsPositionStepped(pt2)) //pt2被对手占
		{nType = NT_DEAD_2;} //死2
		else if(pPlayer->IsPositionStepped(pt2))  //pt2被自己占
		{
			if(pOpponent->IsPositionStepped(pt3))  //pt3被对手占
			{nType = NT_DEAD_3;} //死3
			else if(pPlayer->IsPositionStepped(pt3))  //pt3被自己占
			{
				if(pOpponent->IsPositionStepped(pt4))  //pt4被对手占
				{nType = NT_DEAD_4;}//死4
				else if(pPlayer->IsPositionStepped(pt4))  //pt4被自己占
				{nType = NT_FIVE;}//5连
				else//pt4为空
				{nType = NT_LIVE_4;} //活4
			}
			else//pt3为空
			{nType = NT_LIVE_3;}//活3
		}
		else //pt2为空
		{nType = NT_LIVE_2;}//活二
	}
	else//pt1为空
	{nType = NT_LIVE_1;}//活1
	
	return nType;
}

void CGobangGame::GetNextPoint(AZIMUTH_ANGLE nAzimuth, CPoint & PointNext) const
{
	int nOffset = this->m_pChessboard->GetGridSize();

	switch(nAzimuth)
	{
	case ANGLE_0:
		PointNext.Offset(nOffset, 0);		
		break;
	case ANGLE_45:
		PointNext.Offset(nOffset, -nOffset);
		break;
	case ANGLE_90:
		PointNext.Offset(0, -nOffset);		
		break;
	case ANGLE_135:
		PointNext.Offset(-nOffset, -nOffset);
		break;
	case ANGLE_180:
		PointNext.Offset(-nOffset, 0);
		break;
	case ANGLE_225:
		PointNext.Offset(-nOffset, nOffset);
		break;
	case ANGLE_270:
		PointNext.Offset(0, nOffset);	
		break;
	case ANGLE_315:
		PointNext.Offset(nOffset, nOffset);		
		break;
	default:
		break;
	}
}

int CGobangGame::GetPointScore(const CPlayer * pPlayer, const CPoint & Point) const
{
	NUMBER_TYPE nType[8];
	NUMBER_TYPE nTypeTmp[8];
	int nPointSocre = 0;
	CPoint ptNext[8];

	for(int n = 0; n < 8; n++)
	{
		nType[n] = this->GetPointNT(pPlayer, Point, (AZIMUTH_ANGLE)n);
		if(NT_LIVE_1 == nType[n])
		{
			this->GetNextPoint((AZIMUTH_ANGLE)n, ptNext[n]);
			nTypeTmp[n] = this->GetPointNT(pPlayer, ptNext[n], (AZIMUTH_ANGLE)n);
		}
	}
	for(int n = 0; n < 8; n++)
	{
		if(NT_LIVE_1 == nType[n])
		{
			if(NT_DEAD_4 == nTypeTmp[n])
			{
				for(int m = 0; m < 8; m++)
				{
					if(m == n)
					{continue;}

					if(NT_LIVE_3 == nTypeTmp[m])
					{
						nType[n] = NT_DEAD_4;
						nType[m] = NT_LIVE_3;
						break;
					}
					else if(NT_DEAD_4 == nTypeTmp[m])
					{
						nType[n] = NT_DEAD_4;
						nType[m] = NT_DEAD_4;
						break;
					}
				}
			}
			else if(NT_LIVE_3 == nTypeTmp[n])
			{
				for(int m = 0; m < 8; m++)
				{
					if(m == n)
					{continue;}

					if(NT_DEAD_4 == nTypeTmp[m])
					{
						nType[n] = NT_LIVE_3;
						nType[m] = NT_DEAD_4;
						break;
					}
					else if(NT_LIVE_3 == nTypeTmp[m])
					{
						nType[n] = NT_LIVE_3;
						nType[m] = NT_LIVE_3;
						break;
					}
				}
			}
			/*if(NT_DEAD_4 == nTypeTmp[n])
			{
				if(NT_LIVE_3 == nTypeTmp[n+4])
				{
					nType[n] = NT_DEAD_4;
					nType[n+4] = NT_LIVE_3;
				}
				else if(NT_DEAD_4 == nTypeTmp[n+4])
				{
					nType[n] = NT_DEAD_4;
					nType[n+4] = NT_DEAD_4;
				}
			}
			else if(NT_LIVE_3 == nTypeTmp[n])
			{
				if(NT_DEAD_4 == nType[n+4])
				{
					nType[n] = NT_LIVE_3;
					nType[n+4] = NT_DEAD_4;
				}
				else if(NT_LIVE_3 == nType[n+4])
				{
					nType[n] = NT_LIVE_3;
					nType[n+4] = NT_LIVE_3;
				}
			}*/
		}
	}

	for(int n = 0; n < 4; n++)
	{
		switch(nType[n])
		{
		case NT_DEAD_1:
			switch(nType[n+4])
			{
			case NT_DEAD_1:	
				break;
			case NT_LIVE_1://死1
				nPointSocre += SOCRE_SINGLE_1;
				break;
			case NT_DEAD_2:
				break;
			case NT_LIVE_2://死2
				nPointSocre += SOCRE_SINGLE_2;
				break;
			case NT_DEAD_3:
				break;
			case NT_LIVE_3://死3
				nPointSocre += SOCRE_SINGLE_3;
				break;
			case NT_DEAD_4:
				break;
			case NT_LIVE_4://死4
				nPointSocre += SOCRE_SINGLE_4;
				break;
			case NT_FIVE:
				nPointSocre += SOCRE_WIN_5;
				break;
			default:
				break;
			}
			break;
		case NT_LIVE_1:
			switch(nType[n+4])
			{
			case NT_DEAD_1://死1
				nPointSocre += SOCRE_SINGLE_1;
				break;
			case NT_LIVE_1://活1
				nPointSocre += SOCRE_DOUBLE_1;
				break;
			case NT_DEAD_2://死2
				nPointSocre += SOCRE_SINGLE_2;
				break;
			case NT_LIVE_2://活2
				nPointSocre += SOCRE_DOUBLE_2;
				break;
			case NT_DEAD_3://死3
				nPointSocre += SOCRE_SINGLE_3;
				break;
			case NT_LIVE_3://活3
				nPointSocre += SOCRE_DOUBLE_3;
				break;
			case NT_DEAD_4://死4
				nPointSocre += SOCRE_SINGLE_4;
				break;
			case NT_LIVE_4://活4
				nPointSocre += SOCRE_DOUBLE_4;
				break;
			case NT_FIVE:
				nPointSocre += SOCRE_WIN_5;
				break;
			default:
				break;
			}
			break;
		case NT_DEAD_2:
			switch(nType[n+4])
			{
			case NT_DEAD_1:	
				break;
			case NT_LIVE_1://死2
				nPointSocre += SOCRE_SINGLE_2;
				break;
			case NT_DEAD_2:
				break;
			case NT_LIVE_2://死3
				nPointSocre += SOCRE_SINGLE_3;
				break;
			case NT_DEAD_3:
				break;
			case NT_LIVE_3://死4
				nPointSocre += SOCRE_SINGLE_4;
				break;
			case NT_DEAD_4://5连
			case NT_LIVE_4://5连
			case NT_FIVE:
				nPointSocre += SOCRE_WIN_5;
				break;
			default:
				break;
			}
			break;
		case NT_LIVE_2:
			switch(nType[n+4])
			{
			case NT_DEAD_1://死2
				nPointSocre += SOCRE_SINGLE_2;
				break;
			case NT_LIVE_1://活2
				nPointSocre += SOCRE_DOUBLE_2;
				break;
			case NT_DEAD_2://死3
				nPointSocre += SOCRE_SINGLE_3;
				break;
			case NT_LIVE_2://活3
				nPointSocre += SOCRE_DOUBLE_3;
				break;
			case NT_DEAD_3://死4
				nPointSocre += SOCRE_SINGLE_4;
				break;
			case NT_LIVE_3://活4
				nPointSocre += SOCRE_DOUBLE_4;
				break;
			case NT_DEAD_4://5连
			case NT_LIVE_4://5连
			case NT_FIVE:
				nPointSocre += SOCRE_WIN_5;
				break;
			default:
				break;
			}
			break;
		case NT_DEAD_3:
			switch(nType[n+4])
			{
			case NT_DEAD_1:
				break;
			case NT_LIVE_1://死3
				nPointSocre += SOCRE_SINGLE_3;
				break;
			case NT_DEAD_2:
				break;
			case NT_LIVE_2://死4
				nPointSocre += SOCRE_SINGLE_4;
				break;
			case NT_DEAD_3://5连
			case NT_LIVE_3://5连
			case NT_DEAD_4://5连
			case NT_LIVE_4://5连
			case NT_FIVE:
				nPointSocre += SOCRE_WIN_5;
				break;
			default:
				break;
			}
			break;
		case NT_LIVE_3:
			switch(nType[n+4])
			{
			case NT_DEAD_1:	//死3
				nPointSocre += SOCRE_SINGLE_3;
				break;
			case NT_LIVE_1://活3
				nPointSocre += SOCRE_DOUBLE_3;
				break;
			case NT_DEAD_2:	//死4
				nPointSocre += SOCRE_SINGLE_4;
				break;
			case NT_LIVE_2://活4
				nPointSocre += SOCRE_DOUBLE_4;
				break;
			case NT_DEAD_3:	//5连
			case NT_LIVE_3://5连
			case NT_DEAD_4://5连
			case NT_LIVE_4://5连
			case NT_FIVE:
				nPointSocre += SOCRE_WIN_5;
				break;
			default:
				break;
			}
			break;
		case NT_DEAD_4:
			switch(nType[n+4])
			{
			case NT_DEAD_1:
				break;
			case NT_LIVE_1://死4
				nPointSocre += SOCRE_SINGLE_4;
				break;
			case NT_DEAD_2://5连
			case NT_LIVE_2://5连
			case NT_DEAD_3://5连
			case NT_LIVE_3://5连
			case NT_DEAD_4://5连
			case NT_LIVE_4://5连
			case NT_FIVE:
				nPointSocre += SOCRE_WIN_5;
				break;
			default:
				break;
			}
			break;
		case NT_LIVE_4:
			switch(nType[n+4])
			{
			case NT_DEAD_1:	//死4
				nPointSocre += SOCRE_SINGLE_4;
				break;
			case NT_LIVE_1://活4
				nPointSocre += SOCRE_DOUBLE_4;
				break;
			case NT_DEAD_2://5连
			case NT_LIVE_2://5连
			case NT_DEAD_3://5连
			case NT_LIVE_3://5连
			case NT_DEAD_4://5连
			case NT_LIVE_4://5连
			case NT_FIVE:
				nPointSocre += SOCRE_WIN_5;
				break;
			default:
				break;
			}
			break;
		case NT_FIVE:
			nPointSocre += SOCRE_WIN_5;
			break;
		default:
			break;
		}
	}

	return nPointSocre;
}

/*
**************************************************
* 函数名：防守或者进攻分析
* 所属类：CGobangGame
* 描  述：
* 参  数：pPlayer			分析游戏对象，
*										如pPlayer为游戏者，即为防守分析；而pPlayer为电脑，即为进攻分析
*				 pSocre				返回socre，
*				 pPointGo			返回应该走棋的点 
* 返回值：void
**************************************************
*/
void CGobangGame::Analyze(const CPlayer * pPlayer,  int * pSocre, CPoint * pPointGo) const
{
	CRect rectBoard = this->m_pChessboard->GetChessboardSize();
	int nOffset = this->m_pChessboard->GetGridSize();
	int nPointSocre = 0;
	CPoint pt = CPoint(-1, -1);;

	*pSocre = 0;
	*pPointGo = pt;

	#ifdef _DEBUG
		this->WriteSocre(0, true);	//for debug
	#endif

	for(int y = rectBoard.top; y <= rectBoard.bottom; y += nOffset)
	{
		for(int x = rectBoard.left; x <= rectBoard.right; x += nOffset)
		{
			pt.x = x;
			pt.y = y;

			if(this->IsPositionStepped(pt))
			{
				#ifdef _DEBUG
					this->WriteSocre(0, x == rectBoard.right);
				#endif
				continue;
			}

			nPointSocre = this->GetPointScore(pPlayer, pt);
			if(nPointSocre > *pSocre) 
			{
				*pSocre = nPointSocre;
				*pPointGo = pt;
			}

			#ifdef _DEBUG
				// this->WriteSocre(nPointSocre, x == rectBoard.right);	//for debug			
			#endif
		}	
	}
}

void CGobangGame::RefreshResult(const CPlayer * pWho)
{
	CRect rectBoard = this->m_pChessboard->GetChessboardSize();
	int nOffset = this->m_pChessboard->GetGridSize();
	CPoint pt;
	unsigned int nCount = pWho->GetStepCount();

	if(nCount < 5)
	{return;}

	for(unsigned int n =1; n <= nCount; n++)
	{
		pt = pWho->GetStepPosition(n);

		if(pWho->IsWin(pt, nOffset))
		{
			if(RGB(255,255,255) == pWho->m_pChessman->GetColor())
			{
				this->m_nResult = RESULT_WWIN;
				this->SendShowMessage("白方获胜");
			}
			else if(RGB(0,0,0) == pWho->m_pChessman->GetColor() )
			{
				this->m_nResult = RESULT_BWIN;
				this->SendShowMessage("黑方获胜");
			}
			else
			{
				this->m_nResult = RESULT_CONFIG_ERROR;
				this->SendShowMessage("配置错误");
			}
			
			/*if(!PlaySound("C:\\WINDOWS\\Media\\tada.wav", NULL, SND_FILENAME))
			{
				this->m_nResult = RESULT_RUNTIME_ERROR;
				this->SendShowMessage("运行时错误");
			}*/

			break;
		}
	}
}

bool CGobangGame::Request(LPCSTR strMainMessage)
{
	if(!AfxSocketInit())
	{
		AfxMessageBox("Failed to initialize socket!"); 
		this->SendShowMessage("初始化socket失败");
		return false;
	}

	CSocket Request;

	if(0 ==Request.Create())
	{
		AfxMessageBox("Failed to create socket!");
		this->m_nResult = RESULT_CREATE_FAIL;
		this->SendShowMessage("创建套接字失败");
		WSACleanup();
		return false;
	}

	int nPort;
	if(MODE_SERVER == this->m_nMode)
		nPort = this->m_nPort - 1;
	else if (MODE_CLIENT == this->m_nMode)
		nPort = this->m_nPort + 1;
	else
	{
		this->m_nResult = RESULT_CONFIG_ERROR;
		this->SendShowMessage("配置错误");
		WSACleanup();
		return false;
	}

	if(0 == Request.Connect((MODE_CLIENT == this->m_nMode ? this->m_strServerIPAddress : this->m_strClientIPAddress), nPort))
	{
		AfxMessageBox("Failed to connect to server!");
		this->m_nResult = RESULT_CONNECT_FAIL;
		this->SendShowMessage("连接失败");
		Request.Close();
		WSACleanup();
		return false;
	}


	if(0 == Request.Send(strMainMessage, (int)strlen(strMainMessage)))
	{
		AfxMessageBox("Failed to send data!");
		this->m_nResult = RESULT_SEND_FAIL;
		this->SendShowMessage("发送数据失败");
		Request.Close();
		WSACleanup();
		return false;
	}


	if(0 == strcmp(strMainMessage, "QUIT"))
	{
		Sleep(10);
		if(0 == Request.Send(this->m_strRequestReason, m_strRequestReason.GetLength()))
		{
			AfxMessageBox("Failed to send data!");
			this->m_nResult = RESULT_SEND_FAIL;
			this->SendShowMessage("发送数据失败");
			Request.Close();
			WSACleanup();
			return false;
		}
	}

	this->SendShowMessage("等待对方回应");

	char cBuf[128];
	memset(cBuf, 0, 128);
	if(0 == Request.Receive(cBuf, 128))
	{
		AfxMessageBox("Failed to receive data!");
		this->m_nResult = RESULT_RECEIVE_FAIL;
		this->SendShowMessage("接收数据失败");
		Request.Close();
		WSACleanup();
		return false;
	}

	Request.Close();
	WSACleanup();

	if(0 == strcmp(cBuf, "YES"))  //对方同意请求
		return true;
	else
		return false;
}

void CGobangGame::SendShowMessage(LPCSTR strMsg)
{
	::PostMessageA(this->m_pMainWnd->m_hWnd, UM_SHOW_MESSAGE, 0, (LPARAM)strMsg);

	this->m_pMainWnd->UpdateWindow();
}

UINT CGobangGame::GameHighlightChessman(void)
{
	Sleep(100);
	int nStepCount;
	CPoint ptLastStep;
	while(this->IsGameStart())
	{
		if(this->IsPlayerTurn())
		{
			nStepCount = this->m_pOpponent->GetStepCount();
			if(0 == nStepCount) 
			{
				Sleep(200);
				continue;
			}
			ptLastStep = this->m_pOpponent->GetStepPosition(nStepCount);
			if(!this->m_pOpponent->HightlightStep(ptLastStep)) this->m_nResult = RESULT_RUNTIME_ERROR;
			Sleep(200);
			if(!this->m_pOpponent->UnHightlightStep(ptLastStep)) this->m_nResult = RESULT_RUNTIME_ERROR;
			Sleep(200);
		}
		else if(this->IsOpponentTurn())
		{
			nStepCount = this->m_pPlayer->GetStepCount();
			if(0 == nStepCount) 
			{
				Sleep(200);
				continue;
			}
			ptLastStep = this->m_pPlayer->GetStepPosition(nStepCount);
			if(!this->m_pPlayer->HightlightStep(ptLastStep)) this->m_nResult = RESULT_RUNTIME_ERROR;
			Sleep(200);
			if(!this->m_pPlayer->UnHightlightStep(ptLastStep)) this->m_nResult = RESULT_RUNTIME_ERROR;
			Sleep(200);
		}
		else this->m_nResult = RESULT_RUNTIME_ERROR;
	}
	return 0;
}

/*
**************************************************
* 函数名：SetCursorShape
* 作  用：找到离point最近的棋盘格子交叉点的点，并在该点有效半径内改鼠标形状，
* 所属类：CGobangGame
* 描  述：更改鼠标形状
* 参  数：point		点
*               dValidRadius	鼠标变形有效半径
* 返回值：void
**************************************************
*/
void CGobangGame::SetCursorShape(const CPoint & point) const
{
	int nGridSize = this->m_pChessboard->GetGridSize();
	double dValidRadius = double(nGridSize) / 3;
	CRect rectChessboard = this->m_pChessboard->GetChessboardSize();

	CPoint pt[4];
	pt[0].SetPoint(point.x - point.x % nGridSize, point.y - point.y % nGridSize);
	pt[1].SetPoint(point.x - point.x % nGridSize + nGridSize, point.y - point.y % nGridSize);
	pt[2].SetPoint(point.x - point.x % nGridSize, point.y - point.y % nGridSize + nGridSize);
	pt[3].SetPoint(point.x - point.x % nGridSize + nGridSize, point.y - point.y % nGridSize + nGridSize);
	if(0 != rectChessboard.left % nGridSize)
	{
		pt[0].Offset(rectChessboard.left, 0);
		pt[1].Offset(rectChessboard.left, 0);
		pt[2].Offset(rectChessboard.left, 0);
		pt[3].Offset(rectChessboard.left, 0);
	}
	if(0 != rectChessboard.top % nGridSize)
	{
		pt[0].Offset(0, rectChessboard.top);
		pt[1].Offset(0, rectChessboard.top);
		pt[2].Offset(0, rectChessboard.top);
		pt[3].Offset(0, rectChessboard.top);
	}

	double dDistance[4];
	for(int n = 0; n < 4; ++n)
		dDistance[n] = sqrt(pow(point.x - pt[n].x, 2.0) + pow(point.y - pt[n].y, 2.0)); 

	//设定鼠标按钮形状，条件是：在以交叉点为圆心，dValidRadius为半径的圆内	
	if( point.x >= rectChessboard.left - dValidRadius 
		&& point.x <= rectChessboard.right + dValidRadius 
		&& point.y >= rectChessboard.top - dValidRadius 
		&& point.y <= rectChessboard.bottom + dValidRadius)
	{
		if( (dDistance[0] < dValidRadius) 
			|| (dDistance[1] < dValidRadius) 
			|| (dDistance[3] < dValidRadius) 
			|| (dDistance[3] < dValidRadius) )
		{
			HCURSOR hcur = ::LoadCursor(NULL, IDC_HAND);
			if(hcur)
				::SetCursor(hcur);
		}
	}
}

void CGobangGame::WriteSocre(int nSocre, bool bNewLine) const
{
	CFile file;
	CString strMsg="";
	strMsg.Format("%d\t",nSocre);

	if(file.Open("socre.txt",CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite))
	{
		file.SeekToEnd();
		file.Write(strMsg.GetBuffer(), strMsg.GetLength());
		if(bNewLine)
			file.Write("\r\n", 2);
		file.Close();
		strMsg.ReleaseBuffer();
	}
}

void CGobangGame::WriteMsg(LPCSTR lpstrMsg) const
{
	CTime CurrentTime = CTime::GetCurrentTime();
	CString strDateTime = "";
	strDateTime.Format("%d-%d-%d %s ",CurrentTime.GetYear(), CurrentTime.GetMonth(),CurrentTime.GetDay(), CurrentTime.Format("%H:%M:%S"));

	CFile file("log.txt",CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite);
	file.SeekToEnd();
	file.Write(strDateTime.GetBuffer(), strDateTime.GetLength());
	strDateTime.ReleaseBuffer();

	file.SeekToEnd();
	file.Write(lpstrMsg, (UINT)strlen(lpstrMsg));

	file.SeekToEnd();
	file.Write("\r\n", 2);
	file.Close();
}