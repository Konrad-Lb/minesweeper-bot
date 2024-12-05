#include <Windows.h>

class MineSwpMonitor
{
	public:
		~MineSwpMonitor();
		static MineSwpMonitor* GetInstance();
		static void Destroy();
		HWND GetMinesweepperHWND(); 

	private:
		MineSwpMonitor();

		void RefreshMineSwpHwnd();
		void RefreshMineSwpProcId();
		bool IsProcessAlive() const;
		bool IsHWNDValid() const;
		bool IsWindowEnabled() const;
		bool IsWindowVisible() const;
		bool HasWindowNormalState() const;

		bool FindMineSwpProcHandle();
		bool FindMineSwpHWND();
		static BOOL CALLBACK EnumWindowsProc(HWND windowHandle,LPARAM lParam);

		HWND m_lastMineSwpHwnd;
		HANDLE m_lastMineSwpProcHandle;
		CRITICAL_SECTION m_lock;
		static MineSwpMonitor* m_instance;

};
