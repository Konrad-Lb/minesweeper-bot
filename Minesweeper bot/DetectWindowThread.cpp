#include "stdafx.h"
#include "DetectWindowThread.h"
#include "MineSwpMonitor.h"


DetectWindowThread::DetectWindowThread():Thread(NULL)
{
	m_MineswpHWnd = 0;
	m_MineswpHwndChanged = false;
}

void  DetectWindowThread::Run()
{
	MineSwpMonitor* pMonitor = MineSwpMonitor::GetInstance();
	HWND lastHwnd = 0;

	if(pMonitor != NULL)
	{
		while(!IsStopEventSignaled())
		{
			m_MineswpHWnd = pMonitor->GetMinesweepperHWND();

			if(m_MineswpHWnd != NULL && lastHwnd != NULL && m_MineswpHWnd != lastHwnd)
			{
				m_MineswpHwndChanged = true;
			}
			lastHwnd = m_MineswpHWnd;

			Sleep(200);
		}
	}
}

bool DetectWindowThread::MineSwpWindowChangd()
{
	bool retVal = m_MineswpHwndChanged;
	m_MineswpHwndChanged = false;
	return retVal;
}