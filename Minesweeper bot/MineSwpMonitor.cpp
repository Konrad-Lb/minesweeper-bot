#include "stdafx.h"
#include "MineSwpMonitor.h"
#include <psapi.h>

MineSwpMonitor* MineSwpMonitor::m_instance = NULL;

MineSwpMonitor* MineSwpMonitor::GetInstance()
{
	if(m_instance == NULL)
	{
		m_instance = new MineSwpMonitor;
	}

	return m_instance;
}

void MineSwpMonitor::Destroy()
{
	delete m_instance;
}

MineSwpMonitor::MineSwpMonitor()
:m_lastMineSwpHwnd(NULL),m_lastMineSwpProcHandle(NULL)
{
	InitializeCriticalSection(&m_lock);
}

MineSwpMonitor::~MineSwpMonitor()
{
	CloseHandle(m_lastMineSwpProcHandle);
	DeleteCriticalSection(&m_lock);
}

HWND MineSwpMonitor::GetMinesweepperHWND()
{
	RefreshMineSwpHwnd();
	return m_lastMineSwpHwnd;
}

void MineSwpMonitor::RefreshMineSwpHwnd()
{
	if(!IsHWNDValid() || !(IsWindowVisible() && IsWindowEnabled() && HasWindowNormalState()))
	{
		EnterCriticalSection(&m_lock);
		
		m_lastMineSwpHwnd = NULL;
		RefreshMineSwpProcId();

		LeaveCriticalSection(&m_lock);
	}
}

void MineSwpMonitor::RefreshMineSwpProcId()
{
	if(!(IsProcessAlive() && IsHWNDValid()) )
	{
		CloseHandle(m_lastMineSwpProcHandle);
		m_lastMineSwpProcHandle = NULL;
		FindMineSwpProcHandle();
	}
}

bool MineSwpMonitor::FindMineSwpProcHandle()
{
	bool bResult = false;
	DWORD* pid = new DWORD[1024];
	DWORD pidCnt = 0;

	if(EnumProcesses(pid,sizeof(DWORD)*1024,&pidCnt))
	{
		pidCnt /= sizeof(DWORD);
		for(unsigned int i = 0 ; i < pidCnt ; i++)
		{
			HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION,FALSE,pid[i]);
			if(hProcess != NULL)
			{
				HMODULE hMod = NULL;
				DWORD cbNeeded = 0;
				char processName[256], chBuf[256];

				if(GetProcessImageFileName(hProcess,chBuf,sizeof(chBuf)))
				{
					char* pch = strrchr(chBuf,'\\');
					strcpy_s(processName,pch+1);

					if(strcmp(processName,"winmine.exe") == 0)
					{
						m_lastMineSwpProcHandle = hProcess;
						if(FindMineSwpHWND())
						{
							bResult = true;
							break;
						}
						else
						{
							m_lastMineSwpProcHandle = NULL;
						}
					}
				}
			}
			CloseHandle(hProcess);
		}
	}
	delete [] pid;

	return bResult;
}

bool MineSwpMonitor::FindMineSwpHWND()
{
	DWORD pid = GetProcessId(m_lastMineSwpProcHandle);

	if(pid != 0)
	{
		if(EnumWindows(EnumWindowsProc,(LPARAM)this) == FALSE)
		{
			return true;
		}
	}

	return false;
}

BOOL CALLBACK MineSwpMonitor::EnumWindowsProc(HWND windowHandle,LPARAM lParam)
{
	MineSwpMonitor* pMsm = reinterpret_cast<MineSwpMonitor*>(lParam);
	
	if(pMsm != NULL)
	{
		DWORD searchPid = GetProcessId(pMsm->m_lastMineSwpProcHandle);
		DWORD windowPid = 0;

		if(GetWindowThreadProcessId(windowHandle,&windowPid))
		{
			if(searchPid == windowPid)
			{
				pMsm->m_lastMineSwpHwnd = windowHandle;
				if(pMsm->IsWindowVisible() && pMsm->IsWindowEnabled() && pMsm->HasWindowNormalState())
				{
					return false;
				}
				else
				{
					pMsm->m_lastMineSwpHwnd = 0;
				}
			}
		}
	}
	
	return true;
}

bool MineSwpMonitor::IsProcessAlive() const 
{
	//check if process exists
	if(GetProcessId(m_lastMineSwpProcHandle))
	{
		DWORD procExitCode = 0;
		//is still alive?
		if(GetExitCodeProcess(m_lastMineSwpProcHandle,&procExitCode))
		{
			return procExitCode == STILL_ACTIVE;
		}
	}
	
	return false;
}

bool MineSwpMonitor::IsHWNDValid() const
{	
	if(IsProcessAlive())
	{
		if(IsWindow(m_lastMineSwpHwnd))
		{
			DWORD windowPid = 0;
			DWORD minSwpPid = GetProcessId(m_lastMineSwpProcHandle);
			if(minSwpPid && GetWindowThreadProcessId(m_lastMineSwpHwnd,&windowPid))
			{
				return minSwpPid == windowPid;
			}

		}
	}
	return false;
}
bool MineSwpMonitor::IsWindowEnabled() const
{
	if(IsHWNDValid())
	{
		return ::IsWindowEnabled(m_lastMineSwpHwnd) == TRUE;
	}
	
	return false;
}
bool MineSwpMonitor::HasWindowNormalState() const 
{	
	if(IsHWNDValid())
	{
		LONG windowStyles = GetWindowLong(m_lastMineSwpHwnd,GWL_STYLE);
		
		return !(windowStyles & WS_MINIMIZE);
	}
	
	
	return false;
}

bool MineSwpMonitor::IsWindowVisible() const
{
	if(IsHWNDValid())
	{
		return ::IsWindowVisible(m_lastMineSwpHwnd) == TRUE;
	}

	return false;
}
