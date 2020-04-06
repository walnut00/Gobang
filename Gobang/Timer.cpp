/*
**************************************************
* 文件名：Timer.cpp
* 描  述：
* 作  者：Walnut
* 日  期：2010-9-9 21:11:54
**************************************************
*/
#include "StdAfx.h"
#include "Timer.h"

CTimer::CTimer(void):
			 m_bStop(false)
			,m_bPaused(true)
			,m_timeElapsed(0)
{
	this->m_timeStart = CTime::GetCurrentTime();
}

CTimer::~CTimer(void)
{
}

void CTimer::Reset(void)
{
	this->m_timeStart = CTime::GetCurrentTime();
	this->m_timeElapsed = 0;
}

void CTimer::Pause(void)
{
	if(!this->m_bPaused)
	{
		this->m_bPaused = true;
		this->m_timeElapsed = CTime::GetCurrentTime() - this->m_timeStart;
	}
}

void CTimer::Run(void)
{
	if(this->m_bPaused)
		this->m_timeStart = CTime::GetCurrentTime() -  this->m_timeElapsed;
	this->m_bPaused = false;
}

CTimeSpan CTimer::GetElapsedTime(void) const
{
	if(this->m_bPaused)
	{
		return this->m_timeElapsed;
	}
	else
	{
		CTime timeCurrent = CTime::GetCurrentTime();
		return timeCurrent - this->m_timeStart;
	}
}