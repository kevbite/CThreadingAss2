#pragma once

#include "Thread.h"
namespace kevsoft { 

	/* Pool Thread class
		Used by the thread pool so it has access
		to the Run(RunnableBase* ) and the stopped event 
	*/
	class PThread : public Thread 
	{
	public:
		bool Run(RunnableBase* pClass);
		HANDLE StoppedEventHandle(void);

	};
	
	//Makes the Run(RunnableBase* pClass) public
	bool PThread::Run(RunnableBase* pClass)
	{	
		return Thread::Run(pClass); //call the base method
	}

	//Returns the stopped event handle
	HANDLE PThread::StoppedEventHandle(void)
	{
		return hStoppedEvent_;//return the event handle
	}

}
