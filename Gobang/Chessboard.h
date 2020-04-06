/*
**************************************************
* 文件名：Chessboard.h
* 描  述：
* 作  者：Neil
* 日  期：2010-9-9 21:09:25
**************************************************
*/
#pragma once

class CChessboard
{
public:
	CChessboard(CWnd * pWnd, const CRect & rect, int nGridSize);
	~CChessboard(void);

	void SetBoardSize(const CRect & rect);
	void SetGridSize(int nGridSize);
	void SetBackgroundColor(COLORREF nColor);

	CRect GetBoardSize(void) const;
	CRect GetChessboardSize(void) const;
	int GetGridSize(void) const;
	int GetWidth(void) const;
	int GetHeight(void) const;
	COLORREF GetBackgroundColor(void) const;

	bool Draw(void) const;
private:
	CWnd * m_pWnd;
	CRect m_rectBoard;
	CRect m_rectChessboard;
	int m_nGridSize;
	COLORREF m_nBackgroundColor;
};
