/*
**************************************************
* 文件名：Player.cpp
* 描  述：
* 作  者：Neil
* 日  期：2010-9-9 21:11:43
**************************************************
*/
#include "StdAfx.h"
#include "Player.h"
#include <algorithm>

CPlayer::CPlayer(CWnd * pWnd, int nChessmanRadius, COLORREF nChessmanColor, const CString & strPlayerName) : 
			  m_strPlayerName(strPlayerName)
			, m_bMyTurn(false)
{
	m_pTimer = new CTimer;
	m_pChessman = new CChessman(pWnd, nChessmanRadius, nChessmanColor);
}

CPlayer::~CPlayer(void)
{
	if(NULL != m_pTimer)
	{
		delete m_pTimer;
		m_pTimer = NULL;
	}

	if(NULL != m_pChessman)
	{
		delete m_pChessman;
		m_pChessman = NULL;
	}
}

void CPlayer::SetPlayerName(const CString & strName)
{
	this->m_strPlayerName = strName;
}

CString CPlayer::GetPlayerName(void) const
{
	return this->m_strPlayerName;
}

bool CPlayer::StepGo(const CPoint & pointPosition)
{
	if(this->m_bMyTurn)
	{
		if(this->IsPositionStepped(pointPosition))
		{
			AfxMessageBox("This position already stepped!");
			return false;
		}
		if(!this->m_pChessman->Draw(pointPosition))
		{
			AfxMessageBox("Failed to draw chessman!");
			return false;
		}
		//this->m_stackPoint.Push(pointPosition);
		this->m_vectorPoint.push_back(pointPosition);

		//if(!PlaySound("C:\\WINDOWS\\Media\\start.wav", NULL, SND_FILENAME))
		//{
		//	AfxMessageBox("Failed to play sound!");
		//	return false;
		//}
		//this->m_bMyTurn = false;
	}
	return true;
}

bool CPlayer::StepBack(void)
{
	if(this->m_vectorPoint.empty()) return false;
	this->m_vectorPoint.pop_back();
	return this->RestepAll();
}

void CPlayer::EraseStep(void)
{
	this->m_vectorPoint.clear();
}

unsigned int CPlayer::GetStepCount(void) const
{
	return (unsigned int)this->m_vectorPoint.size();
}

CPoint CPlayer::GetStepPosition(unsigned int nStep) const
{
	VERIFY(nStep > 0 && !this->m_vectorPoint.empty());
	return this->m_vectorPoint[nStep-1];
}

bool CPlayer::RestepAll(void)
{
	for(unsigned int n = 0; n < this->GetStepCount(); n++)
	{
		if(!this->m_pChessman->Draw(this->m_vectorPoint[n]))	
			return false;
	}	
	return true;
}

bool CPlayer::IsPositionStepped(const CPoint & pointPosition) const
{
	std::vector<CPoint>::const_iterator pIter = std::find(this->m_vectorPoint.begin(), this->m_vectorPoint.end(), pointPosition);
	return (pIter != this->m_vectorPoint.end());
}

bool CPlayer::RequestStepback(void) const
{
	return (IDOK== AfxMessageBox("确认要要发送悔棋请求吗?", MB_ICONQUESTION | MB_OKCANCEL));
}

bool CPlayer::RequestGiveup(void) const
{
	return (IDOK== AfxMessageBox("确认要发送认输请求吗?", MB_ICONQUESTION | MB_OKCANCEL));
}

bool CPlayer::RequestQuit(void) const
{
	return (IDOK== AfxMessageBox("确认要退出游戏吗?", MB_ICONQUESTION | MB_OKCANCEL));
}

void CPlayer::SetMyTurn(bool bTurn)
{
	this->m_bMyTurn = bTurn;
}

bool CPlayer::IsMyTurn(void) const
{
	return this->m_bMyTurn;
}

/*
**************************************************
* 函数名：IsWin
* 所属类：CPlayer
* 描  述：根据某个点和offset值，判读是否形成5连
* 参  数：pointPosition要评估的点，nOffset棋盘格子大小
* 返回值：true - 点pointPosition形成5连，否则false
**************************************************
*/
bool CPlayer::IsWin(const CPoint & pointPosition, int nOffset) const
{
	CPoint pt1= pointPosition;
	CPoint pt2 = pointPosition;
	CPoint pt3 = pointPosition;
	CPoint pt4 = pointPosition;

	//0
	pt1.Offset(nOffset, 0);
	pt2.Offset(nOffset * 2, 0);
	pt3.Offset(nOffset * 3, 0);
	pt4.Offset(nOffset * 4, 0);

	if(	this->IsPositionStepped(pointPosition)
		&& this->IsPositionStepped(pt1) 
		&& this->IsPositionStepped(pt2) 
		&& this->IsPositionStepped(pt3) 
		&& this->IsPositionStepped(pt4) ) return true;

	pt1 = pointPosition;
	pt2 = pointPosition;
	pt3 = pointPosition;
	pt4 = pointPosition;
	
	//45
	pt1.Offset(nOffset, -nOffset);
	pt2.Offset(nOffset * 2, -nOffset * 2);
	pt3.Offset(nOffset * 3, -nOffset * 3);
	pt4.Offset(nOffset * 4, -nOffset * 4);

	if(	this->IsPositionStepped(pointPosition)
		&& this->IsPositionStepped(pt1) 
		&& this->IsPositionStepped(pt2) 
		&& this->IsPositionStepped(pt3) 
		&& this->IsPositionStepped(pt4) ) return true;


	pt1 = pointPosition;
	pt2 = pointPosition;
	pt3 = pointPosition;
	pt4 = pointPosition;

	//90
	pt1.Offset(0, -nOffset);
	pt2.Offset(0, -nOffset * 2);
	pt3.Offset(0, -nOffset * 3);
	pt4.Offset(0, -nOffset * 4);

	if(	this->IsPositionStepped(pointPosition)
		&& this->IsPositionStepped(pt1) 
		&& this->IsPositionStepped(pt2) 
		&& this->IsPositionStepped(pt3) 
		&& this->IsPositionStepped(pt4) ) return true;

	pt1 = pointPosition;
	pt2 = pointPosition;
	pt3 = pointPosition;
	pt4 = pointPosition;

	//135
	pt1.Offset(-nOffset, -nOffset);
	pt2.Offset(-nOffset * 2, -nOffset * 2);
	pt3.Offset(-nOffset * 3, -nOffset * 3);
	pt4.Offset(-nOffset * 4, -nOffset * 4);

	if(	this->IsPositionStepped(pointPosition)
		&& this->IsPositionStepped(pt1) 
		&& this->IsPositionStepped(pt2) 
		&& this->IsPositionStepped(pt3) 
		&& this->IsPositionStepped(pt4) ) return true;

	pt1 = pointPosition;
	pt2 = pointPosition;
	pt3 = pointPosition;
	pt4 = pointPosition;

	//180
	pt1.Offset(-nOffset, 0);
	pt2.Offset(-nOffset * 2, 0);
	pt3.Offset(-nOffset * 3, 0);
	pt4.Offset(-nOffset * 4, 0);

	if(	this->IsPositionStepped(pointPosition)
		&& this->IsPositionStepped(pt1) 
		&& this->IsPositionStepped(pt2) 
		&& this->IsPositionStepped(pt3) 
		&& this->IsPositionStepped(pt4) ) return true;

	pt1 = pointPosition;
	pt2 = pointPosition;
	pt3 = pointPosition;
	pt4 = pointPosition;

	//225
	pt1.Offset(-nOffset, nOffset);
	pt2.Offset(-nOffset * 2, nOffset * 2);
	pt3.Offset(-nOffset * 3, nOffset * 3);
	pt4.Offset(-nOffset * 4, nOffset * 4);

	if(	this->IsPositionStepped(pointPosition)
		&& this->IsPositionStepped(pt1) 
		&& this->IsPositionStepped(pt2) 
		&& this->IsPositionStepped(pt3) 
		&& this->IsPositionStepped(pt4) ) return true;

	pt1 = pointPosition;
	pt2 = pointPosition;
	pt3 = pointPosition;
	pt4 = pointPosition;

	//270
	pt1.Offset(0, nOffset);
	pt2.Offset(0, nOffset * 2);
	pt3.Offset(0, nOffset * 3);
	pt4.Offset(0, nOffset * 4);

	if(	this->IsPositionStepped(pointPosition)
		&& this->IsPositionStepped(pt1) 
		&& this->IsPositionStepped(pt2) 
		&& this->IsPositionStepped(pt3) 
		&& this->IsPositionStepped(pt4) ) return true;

	pt1 = pointPosition;
	pt2 = pointPosition;
	pt3 = pointPosition;
	pt4 = pointPosition;

	//315
	pt1.Offset(nOffset, nOffset);
	pt2.Offset(nOffset * 2, nOffset * 2);
	pt3.Offset(nOffset * 3, nOffset * 3);
	pt4.Offset(nOffset * 4, nOffset * 4);
	
	if(	this->IsPositionStepped(pointPosition)
		&& this->IsPositionStepped(pt1) 
		&& this->IsPositionStepped(pt2) 
		&& this->IsPositionStepped(pt3) 
		&& this->IsPositionStepped(pt4) ) return true;
	
	return false;
}

bool CPlayer::HightlightLastStep(void) const
{
	int nStep = this->GetStepCount();
	if(0 == nStep) return true;

	CPoint pt = this->GetStepPosition(nStep);
	return this->m_pChessman->Draw(pt, true);
}

bool CPlayer::UnHightlightLastStep(void) const
{
	int nStep = this->GetStepCount();
	if(0 == nStep) return true;

	CPoint pt = this->GetStepPosition(nStep);
	return this->m_pChessman->Draw(pt);
}

bool CPlayer::HightlightStep(const CPoint & pointPosition) const
{
	if(!this->IsPositionStepped(pointPosition)) return true; //the player may requested step back, the pointPosition already removed from the vector
	return this->m_pChessman->Draw(pointPosition, true);
}

bool CPlayer::UnHightlightStep(const CPoint & pointPosition) const
{
	if(!this->IsPositionStepped(pointPosition)) return true; //the player may requested step back, the pointPosition already removed from the vector
	return this->m_pChessman->Draw(pointPosition);
}