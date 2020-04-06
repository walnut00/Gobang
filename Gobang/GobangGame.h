/*
**************************************************
* 文件名：GobangGame.h
* 描  述：
* 作  者：Neil
* 日  期：2010-11-22 10:25:26
**************************************************
*/
#pragma once
#include "GobangDefine.h"
#include "Chessboard.h"
#include "Chessman.h"
#include "Player.h"



class CGobangGame
{
public:
	CGobangGame(CWnd * pWnd, const CRect & rectChessboard, int nGridSize);
	~CGobangGame(void);
	
	void GameInitialize();
	void GameSetup(void);
	void GameStart(void);

	bool PlayerStep(const CPoint & pointPosition);
	bool IsPositionStepped(const CPoint & pointPosition) const;
	void StepBack(void);
	void GiveUp(void);
	bool Quit(void);

	void SetSingleMode(void);
	void SetDoubleMode(void);
	void SetCursorShape(const CPoint & point) const;

	CString GetOpponentName(void) const;
	CString GetPlayerName(void) const;

	void SetPlayerChessmanColor(COLORREF nMyColor);
	COLORREF GetPlayerChessmanColor(void) const;
	COLORREF GetOpponentChessmanColor(void) const;

	void SetChessboardBackgroundColor(COLORREF nMyColor);
	void SetChessboardBackgroundColor(void);
	COLORREF GetChessboardBackgroundColor(void) const;

	CTimeSpan GetPlayerSpendingTime(void) const;
	CTimeSpan GetOpponentSpendingTime(void) const;

	UINT GetPlayerStepCount(void) const;
	UINT GetOpponentStepCount(void) const;

	bool IsPlayerTurn(void) const;
	bool IsOpponentTurn(void) const;

	GAME_RESULT GetResult(void) const;
	bool IsGameStart(void) const;

	CString GetGameVersion(void) const;

private:
	UINT GameSingle(void); //single mode main thread
	UINT GameServer(void); //double mode server main thread
	UINT GameClient(void); //double mode client main thread
	UINT GameListen(void); //double mode client listen thread
	UINT GameStepback(void); //double mode server or client step back thread
	UINT GameGiveup(void); //double mode server or client give up thread
	UINT GameEscape(void); //double mode server or client escape thread
	UINT GameQuit(void); //double mode server or client force quit thread
	UINT GameReply(CSocket * pSocket); //double mode server or client socket reply thread
	UINT GameHighlightChessman(void); //Highlight the last go point

	static UINT ThreadSingle( LPVOID pParam );
	static UINT ThreadServer( LPVOID pParam );
	static UINT ThreadClient( LPVOID pParam );
	static UINT ThreadStepBack(LPVOID pParam);
	static UINT ThreadListen(LPVOID pParam);
	static UINT ThreadGiveup(LPVOID pParam);
	static UINT ThreadEscape(LPVOID pParam);
	static UINT ThreadQuit(LPVOID pParam);
	static UINT ThreadHightlight(LPVOID pParam);
	
	bool Config(void);
	
	bool OpponentStep(const CPoint & pointPosition);

	void SetPlayerTurn(void);
	void SetOpponentTurn(void);
	void SetOpponentChessmanColor(COLORREF nMyColor);
	
	NUMBER_TYPE GetPointNT(const CPlayer * pPlayer, const CPoint & Point, AZIMUTH_ANGLE nAzimuth) const;
	int GetPointScore(const CPlayer * pPlayer, const CPoint & Point) const;
	void GetNextPoint(AZIMUTH_ANGLE nAzimuth, CPoint & PointNext) const;
	void Analyze(const CPlayer * pPlayer, int * pSocre, CPoint * pPointGo) const;

	void RefreshResult(const CPlayer * pWho);
	void GameOver(void);

	bool Request(LPCSTR strMainMessage); //socket send message to server or client
	void SendShowMessage(LPCSTR strMsg);

	void WriteSocre(int nSocre, bool bNewLine = false) const;
	void WriteMsg(LPCSTR lpstrMsg) const;

private:
	CWnd * m_pMainWnd;	
	CChessboard * m_pChessboard;
	
	CPlayer * m_pPlayer;
	CPlayer * m_pOpponent;
	
	UINT m_nPort; //double mode, server listen or client connection port
	CString m_strServerIPAddress;
	CString m_strClientIPAddress;
	CString m_strRequestReason;

	GAME_MODE m_nMode;	
	GAME_LEVEL m_nLevel;
	GAME_RESULT m_nResult;

	bool m_bStart;
	bool m_bStepback;
	bool m_bPlayerGiveup;
	bool m_bOpponentGiveup;
	bool m_bPlayerEscape;
	bool m_bOpponentEscape;
	bool m_bWaitting;
	CWinThread * m_pThreadListen;
};

