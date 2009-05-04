#pragma once

#include "Thread.h"
namespace kevsoft { 
	class PThread : public Thread 
	{
	public:
		bool Run(ThreadFunctorBase* pClass);
		HANDLE RunningEventHandle(void);

	};

	bool PThread::Run(ThreadFunctorBase* pClass)
	{	
		return Thread::Run(pClass);
	}

	HANDLE PThread::RunningEventHandle(void)
	{
		return hRunningEvent_;
	}

}
