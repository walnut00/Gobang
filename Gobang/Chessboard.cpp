/*
**************************************************
* 文件名：Chessboard.cpp
* 描  述：
* 作  者：Walnut
* 日  期：2010-9-9 21:11:06
**************************************************
*/
#include "StdAfx.h"
#include "Chessboard.h"

CChessboard::CChessboard(CWnd * pWnd, const CRect & rect, int nGridSize) :
						 m_rectBoard(rect)
						,m_nGridSize(nGridSize)
						,m_nBackgroundColor(RGB(0xE4,0xC3,0x92))
						,m_pWnd(pWnd)
{
#ifdef _DEBUG
	ASSERT(0 < m_nGridSize);
	ASSERT(0 == rect.Width() % nGridSize && 0 == rect.Height() % nGridSize);
#else
	VERIFY(0 < m_nGridSize);
	VERIFY(0 == rect.Width() % nGridSize && 0 == rect.Height() % nGridSize);
#endif

	this->m_rectChessboard.left = this->m_rectBoard.left + this->m_nGridSize;
	this->m_rectChessboard.right = this->m_rectBoard.right - this->m_nGridSize;
	this->m_rectChessboard.top = this->m_rectBoard.top + this->m_nGridSize;
	this->m_rectChessboard.bottom = this->m_rectBoard.bottom - this->m_nGridSize;
}

CChessboard::~CChessboard(void)
{
}

CRect CChessboard::GetBoardSize(void) const
{
	return this->m_rectBoard;
}

CRect CChessboard::GetChessboardSize(void) const
{
	return this->m_rectChessboard;
}

void CChessboard::SetBoardSize(const CRect & rect)
{
	this->m_rectBoard = rect;
}

void CChessboard::SetGridSize(int nGridSize)
{
	this->m_nGridSize = nGridSize;
}

void CChessboard::SetBackgroundColor(COLORREF nColor)
{
	this->m_nBackgroundColor = nColor;
	this->m_pWnd->Invalidate();
}

int CChessboard::GetGridSize(void) const
{
	return this->m_nGridSize;
}

int CChessboard::GetWidth(void) const
{
	return this->m_rectBoard.Width();
}

int CChessboard::GetHeight(void) const
{
	return this->m_rectBoard.Height();
}

COLORREF CChessboard::GetBackgroundColor(void) const
{
	return this->m_nBackgroundColor;
}

bool CChessboard::Draw(void) const
{
	CClientDC dc(this->m_pWnd);

	CBrush NewBrush(this->m_nBackgroundColor);
	dc.FillRect(this->m_rectBoard, &NewBrush);

	for(int x = this->m_rectChessboard.left; x <= this->m_rectChessboard.right; x += this->m_nGridSize)
	{
		for(int y = this->m_rectChessboard.top; y <= this->m_rectChessboard.bottom; y += this->m_nGridSize)
		{
			dc.MoveTo(x, y);
			if(!dc.LineTo(this->m_rectChessboard.right, y)) return false; //先画x轴

			dc.MoveTo(x, y);
			if(!dc.LineTo(x, this->m_rectChessboard.bottom)) return false; //然后画y轴
		}
	}

	char chr = 'A';
	dc.SetBkMode(TRANSPARENT);
	for(int x = this->m_rectChessboard.left - 3; x <= this->m_rectChessboard.right; x += this->m_nGridSize)
	{
		if(!dc.TextOutA(x, this->m_rectBoard.top ,  &chr, 1)) return false;
		chr ++;
	}

	int ny = 0;
	CString strShow = "";
	
	for(int y = this->m_rectChessboard.top; y <= this->m_rectChessboard.bottom; y += this->m_nGridSize)
	{
		strShow.Format("%.2d", ny);
		if(!dc.TextOutA(this->m_rectBoard.left, y - 9,  strShow)) return false;
		ny ++;
	}
	return true;
}