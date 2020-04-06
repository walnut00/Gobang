/*
**************************************************
* 文件名：GobangDlg.h
* 描  述：
* 作  者：Walnut
* 日  期：2010-9-9 21:10:22
**************************************************
*/

// GobangDlg.h : 头文件
//

#pragma once
#include "atltypes.h"
#include "LinkStack.h"
#include "GobangGame.h"
#include "afxwin.h"
// CGobangDlg 对话框
class CGobangDlg : public CDialog
{
// 构造
public:
	CGobangDlg(CWnd* pParent = NULL);	// 标准构造函数
	~CGobangDlg();
// 对话框数据
	enum { IDD = IDD_GOBANG_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()	
private:
	int nX;//window's width
	int nY; //window's heigh

	CRect rectChessboard;//棋盘的大小（矩形）
	int nGridSize;//格子大小
	CGobangGame * m_pGobang;

public:
	afx_msg void OnBnClickedButtonStartGame();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT OnShowMessage(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedButtonStepBack();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedButtonGiveUp();

	void SetGridWidth(int nWidth);
	void ValidateButton(bool bEable = true);

	static UINT ThreadResult( LPVOID pParam );
	afx_msg void OnClose();
};
