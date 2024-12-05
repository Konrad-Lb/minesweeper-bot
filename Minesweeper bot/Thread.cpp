#include "stdafx.h"
#include "Thread.h"
#include <process.h>

Thread::Thread(void* param):m_parameters(param)
{
	m_stopEvent = CreateEvent(0,TRUE,FALSE,0);
}

Thread::~Thread()
{
	CloseHandle(m_stopEvent);
}

bool Thread::Start()
{
	ResetEvent(m_stopEvent);
	m_threadHandle = (HANDLE)_beginthread(&StaticThreadMetod,0,this);
	return true;
}

void Thread::Terminate()
{
	SetEvent(m_stopEvent);
}

void  Thread::StaticThreadMetod(void* data)
{
	Thread* t = reinterpret_cast<Thread*>(data);
	t->Run();
}

bool Thread::IsStopEventSignaled(DWORD dwTimeOut)
{
	if (WaitForSingleObject(m_stopEvent, dwTimeOut) == WAIT_OBJECT_0) 
	{
		return true;
	}
	else 
	{
		return false;
	}
}