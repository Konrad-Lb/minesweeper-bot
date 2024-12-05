// Miesweeper bot.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>
#include <process.h>

#include "DetectWindowThread.h"
#include "PlayThread.h"
#include <iostream>


int _tmain(int argc, _TCHAR* argv[])
{
	bool windowDetected = false;
	DetectWindowThread dwp;
	PlayThread pt;
	dwp.Start();

	while(1)
	{
		bool isWindow = dwp.MineSwpWindowDetected();
		bool windowChanged = dwp.MineSwpWindowChangd();

		if(isWindow != windowDetected)
		{
			if(isWindow)
			{
				std::cout << "Window detected" << std::endl;
				pt.Start();
			}
			else
			{
				std::cout << "Window not detected" << std::endl;
				pt.Terminate();
			}
			
			windowDetected = isWindow;
		}
		if(windowChanged)
		{
			std::cout << "Window HWND changed" << std::endl;
		}

		Sleep(100);
	}

	return 0;
}

