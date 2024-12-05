#ifndef THREAD_H
#define THREAD_H

#include <Windows.h>

class Thread
{
	public:
		Thread(void* param);
		virtual ~Thread();
		bool Start();
		void Terminate();

	private:
		static void StaticThreadMetod(void* data);

	protected:
		virtual void  Run() = 0;
		bool IsStopEventSignaled(DWORD dwTimeOut = 10);
		
		HANDLE m_threadHandle;
		HANDLE m_stopEvent;
		void* m_parameters;

};

#endif
