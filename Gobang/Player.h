/*
**************************************************
* 文件名：Player.h
* 描  述：
* 作  者：Neil
* 日  期：2010-9-9 21:10:45
**************************************************
*/
#pragma once
#include <vector>
#include "Chessman.h"
#include "Timer.h"

class CPlayer
{
public:
	CPlayer(CWnd * pWnd, int nChessmanRadius, COLORREF nChessmanColor, const CString & strPlayerName);
	~CPlayer(void);
	
	void SetPlayerName(const CString & strName);
	CString GetPlayerName(void) const;

	bool StepGo(const CPoint & pointPosition);
	bool StepBack(void);
	bool RestepAll(void);
	void EraseStep(void);

	unsigned int GetStepCount(void) const;
	CPoint GetStepPosition(unsigned int nStep) const;
	bool IsPositionStepped(const CPoint & pointPosition) const;

	bool RequestStepback(void) const;
	bool RequestGiveup(void) const;
	bool RequestQuit(void) const;

	void SetMyTurn(bool bTurn = true);
	bool IsMyTurn(void) const;
	bool IsWin(const CPoint & pointPosition, int nOffset) const;

	bool HightlightLastStep(void) const;
	bool UnHightlightLastStep(void) const;
	bool HightlightStep(const CPoint & pointPosition) const;
	bool UnHightlightStep(const CPoint & pointPosition) const;
public:
	CTimer * m_pTimer;
	CChessman * m_pChessman;

private:
	CString m_strPlayerName;
	std::vector<CPoint> m_vectorPoint;
	bool m_bMyTurn;
};
