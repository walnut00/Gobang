/*
**************************************************
* �ļ�����Chessman.h
* ��  ����
* ��  �ߣ�Neil
* ��  �ڣ�2010-9-9 21:09:41
**************************************************
*/
#pragma once
#include "LinkStack.h"

class CChessman
{
public:
	CChessman(CWnd * pWnd, int nRadius, COLORREF nColor);
	virtual ~CChessman(void);
	
	void SetColor(COLORREF nColor);
	void SetRadius(int nRadius);

	COLORREF GetColor(void) const;
	int GetRadius(void) const;
	bool Draw(const CPoint & pointPosition, bool bHightlight = false);
	
private:
	CWnd * m_pWnd;
	int m_nRadius;
	COLORREF m_nColor;
};
