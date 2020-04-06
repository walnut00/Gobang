/*
**************************************************
* �ļ�����GobangDlg.h
* ��  ����
* ��  �ߣ�Walnut
* ��  �ڣ�2010-9-9 21:10:22
**************************************************
*/

// GobangDlg.h : ͷ�ļ�
//

#pragma once
#include "atltypes.h"
#include "LinkStack.h"
#include "GobangGame.h"
#include "afxwin.h"
// CGobangDlg �Ի���
class CGobangDlg : public CDialog
{
// ����
public:
	CGobangDlg(CWnd* pParent = NULL);	// ��׼���캯��
	~CGobangDlg();
// �Ի�������
	enum { IDD = IDD_GOBANG_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()	
private:
	int nX;//window's width
	int nY; //window's heigh

	CRect rectChessboard;//���̵Ĵ�С�����Σ�
	int nGridSize;//���Ӵ�С
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
