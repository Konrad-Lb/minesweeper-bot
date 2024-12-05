#include "Thread.h"

class DetectWindowThread : public Thread
{
	public:
		DetectWindowThread();
		bool MineSwpWindowDetected()  { return m_MineswpHWnd != 0;	}
		bool MineSwpWindowChangd();

	private:
		HWND m_MineswpHWnd;
		bool m_MineswpHwndChanged;

	protected:
		virtual void Run();
};