#include "Thread.h"

class PlayThread : public Thread
{
	public:
		PlayThread():Thread(NULL),m_gameArray(NULL) {}
		virtual ~PlayThread();

	private:
		struct SArrayField
		{
			int row;
			int col;
		};
		
		
		void LeftMouseClick(const POINT& p);
		POINT GetCellCoords(int row, int col) const;
		bool ReadMineSwpGameSize();
		bool ReadMineSwpGameArray();

		HWND m_hwnd;
		int m_rowCnt;
		int m_colCnt;
		unsigned char **m_gameArray;


	protected:
		virtual void Run();

};
