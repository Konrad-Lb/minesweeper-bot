#include "stdafx.h"
#include <windows.h>
#include <windef.h>
#include <vector>
#include <cstdlib>
#include <ctime>
#include "PlayThread.h"
#include "MineSwpMonitor.h"

PlayThread::~PlayThread()
{
	for(int i = 0 ; i < m_rowCnt ; i++)
	{
		delete [] m_gameArray[i];
	}
	delete [] m_gameArray;
}

void PlayThread::Run()
{
	Sleep(1000);
	MineSwpMonitor* pMonitor = MineSwpMonitor::GetInstance();
	if(pMonitor != NULL)
	{
		srand((unsigned int)time(0));
		m_hwnd = pMonitor->GetMinesweepperHWND();
		if(m_hwnd != NULL)
		{
			int i = 0;
			int j = 0;

			ReadMineSwpGameSize();

			m_gameArray = new unsigned char* [m_rowCnt];
			for(int i = 0 ; i < m_rowCnt ; i++)
			{
				m_gameArray[i] = new unsigned char[m_colCnt];
			}
			std::vector<SArrayField> v;
			for(int i = 0 ; i < m_rowCnt ; i++)
				for(int j = 0 ; j < m_colCnt ; j++)
				{
					SArrayField tmp;
					tmp.row = i;
					tmp.col = j;

					v.push_back(tmp);
				}
		
			if(SetForegroundWindow(m_hwnd)== TRUE)
			{
				while(!IsStopEventSignaled())
				{
				//POINT cursorPos;
					HWND currHwnd = pMonitor->GetMinesweepperHWND();
					if(currHwnd != m_hwnd)
					{
						break;
					}

					currHwnd = GetForegroundWindow();
					if(currHwnd != m_hwnd)
					{
						break;
					}

					/*if(i > 0 && j > 0)
					{
						if(GetCursorPos(&cursorPos))
						{
							currHwnd = WindowFromPoint(cursorPos);
							if(currHwnd != m_hwnd)
							{
								break;
							}
						}
						else
						{
							break;
						}
					}*/

					ReadMineSwpGameArray();

					int r = rand() % v.size();
					i = v[r].row;
					j = v[r].col;

					v.erase(v.begin()+r);
					
					POINT p = GetCellCoords(i,j);
					if(m_gameArray[i][j] == 0x0F)
					{
						LeftMouseClick(p);
						//Sleep(25);
					}

					if(v.empty())
						break;

					/*j++;
					if(j >= m_colCnt)
					{
						j = 0;
						i++;
					}
					if(i >= m_rowCnt)
						break;*/
				}
			}
		}
	}

}

void PlayThread::LeftMouseClick(const POINT& p)
{

	POINT screenPoint = p;
	
	ClientToScreen(m_hwnd,&screenPoint);
	SetCursorPos(screenPoint.x,screenPoint.y);
	SendMessage(m_hwnd,WM_LBUTTONDOWN,MK_LBUTTON,MAKELPARAM(p.x,p.y));
	SendMessage(m_hwnd,WM_LBUTTONUP,MK_LBUTTON,MAKELPARAM(p.x,p.y));
}

POINT PlayThread::GetCellCoords(int row, int col) const
{
	int maxRow = 24;
	int maxCol = 30;
	int startTop = 20;
	int startLeft = 62;
	POINT result;

	result.y = startLeft + 16*row;
	result.x = startTop + 16*col;

	return result;

}

bool PlayThread::ReadMineSwpGameSize()
{
	bool bResult = false;
	DWORD pid = 0;
	GetWindowThreadProcessId(m_hwnd,&pid);

	HANDLE hProcess =  OpenProcess(PROCESS_VM_READ | PROCESS_VM_OPERATION,FALSE,pid);	
	
	if(hProcess)
	{
		if(ReadProcessMemory(hProcess,(void*)0x010056AC,&m_colCnt,sizeof(m_colCnt),NULL))
		{
			if(ReadProcessMemory(hProcess,(void*)0x010056A8,&m_rowCnt,sizeof(m_rowCnt),NULL))
			{
				bResult = true;
			}
		}
	}
	CloseHandle(hProcess);

	return bResult;
}

bool PlayThread::ReadMineSwpGameArray()
{
	bool bResult = false;

	if(m_gameArray != NULL)
	{
		DWORD pid = 0;
		GetWindowThreadProcessId(m_hwnd,&pid);

		HANDLE hProcess = OpenProcess(PROCESS_VM_READ | PROCESS_VM_OPERATION,FALSE,pid);
		if(hProcess != NULL)
		{
			int baseAddr = 0;
			for(int i = 0 ; i < m_rowCnt ; i++)
			{
				if(ReadProcessMemory(hProcess,(void*)(0x01005361 + baseAddr),m_gameArray[i],sizeof(char)*m_colCnt,NULL))
				{
					baseAddr += 0x20;
				}
			}
		}
		CloseHandle(hProcess);
	}
	return bResult;
}

