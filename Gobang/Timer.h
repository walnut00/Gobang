/*
**************************************************
* 文件名：Timer.h
* 描  述：
* 作  者：Walnut
* 日  期：2010-9-9 21:10:52
**************************************************
*/
#pragma once

class CTimer
{
public:
	CTimer(void);
	~CTimer(void);

	void Reset(void);
	void Pause(void);
	void Run(void);
	CTimeSpan GetElapsedTime(void) const;

private:
	bool m_bStop;
	bool m_bPaused;

	CTime m_timeStart;
	CTimeSpan m_timeElapsed;
};
