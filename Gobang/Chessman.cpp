/*
**************************************************
* 文件名：Chessman.cpp
* 描  述：
* 作  者：Walnut
* 日  期：2010-9-9 21:11:11
**************************************************
*/
#include "StdAfx.h"
#include "Chessman.h"

CChessman::CChessman(CWnd * pWnd, int nRadius, COLORREF nColor) : 
					 m_pWnd(pWnd)
					,m_nRadius(nRadius)
					,m_nColor(nColor)
{
#ifdef _DEBUG
	ASSERT(0 < m_nRadius);
#else
	VERIFY(0 < m_nRadius);
#endif
}

CChessman::~CChessman(void)
{
}

void CChessman::SetColor(COLORREF nColor)
{
	this->m_nColor = nColor;
}

COLORREF CChessman::GetColor(void) const
{
	return this->m_nColor;
}

void CChessman::SetRadius(int nRadius)
{
	this->m_nRadius = nRadius;
}

int CChessman::GetRadius(void) const
{
	return this->m_nRadius;
}

bool CChessman::Draw(const CPoint & pointPosition, bool bHightlight)
{
	CClientDC dc(this->m_pWnd);
	CBrush NewBrush(this->m_nColor);
	CBrush * pOldBrush = dc.SelectObject(&NewBrush);

	if(!pOldBrush) return false;
	if(0 == dc.Ellipse(pointPosition.x - m_nRadius, pointPosition.y - m_nRadius, pointPosition.x + m_nRadius, pointPosition.y +m_nRadius)) return false;
	if(NULL == dc.SelectObject(pOldBrush)) return false;
	
	if(bHightlight)
	{
		COLORREF clrTmp = RGB(255, 0, 0);
		CPen NewPen(PS_SOLID, 1, clrTmp);
		CPen * pOldPen = dc.SelectObject(&NewPen);
		if(!pOldPen) return false;

		int nGridSize = this->m_nRadius;
		dc.MoveTo(pointPosition.x - nGridSize / 2, pointPosition.y - nGridSize / 2);
		dc.LineTo(pointPosition.x - nGridSize / 4, pointPosition.y - nGridSize / 2);

		dc.MoveTo(pointPosition.x - nGridSize / 2, pointPosition.y - nGridSize / 2);
		dc.LineTo(pointPosition.x - nGridSize / 2, pointPosition.y - nGridSize / 4);

		dc.MoveTo(pointPosition.x + nGridSize / 2, pointPosition.y - nGridSize / 2);
		dc.LineTo(pointPosition.x + nGridSize / 4, pointPosition.y - nGridSize / 2);

		dc.MoveTo(pointPosition.x + nGridSize / 2, pointPosition.y - nGridSize / 2);
		dc.LineTo(pointPosition.x + nGridSize / 2, pointPosition.y - nGridSize / 4);

		dc.MoveTo(pointPosition.x + nGridSize / 2, pointPosition.y + nGridSize / 2);
		dc.LineTo(pointPosition.x + nGridSize / 4, pointPosition.y + nGridSize / 2);

		dc.MoveTo(pointPosition.x + nGridSize / 2, pointPosition.y + nGridSize / 2);
		dc.LineTo(pointPosition.x + nGridSize / 2, pointPosition.y + nGridSize / 4);

		dc.MoveTo(pointPosition.x - nGridSize / 2, pointPosition.y + nGridSize / 2);
		dc.LineTo(pointPosition.x - nGridSize / 4, pointPosition.y + nGridSize / 2);

		dc.MoveTo(pointPosition.x - nGridSize / 2, pointPosition.y + nGridSize / 2);
		dc.LineTo(pointPosition.x - nGridSize / 2, pointPosition.y + nGridSize / 4);

		if(NULL == dc.SelectObject(pOldPen)) return false;
	}
	return true;
}