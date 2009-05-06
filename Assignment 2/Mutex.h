#pragma once
#include <windows.h>
#include <process.h>

namespace kevsoft {

	class Mutex
	{
	private:
		CRITICAL_SECTION cs_;
	public:

		Mutex(void)
		{
			// Initialize the critical section
			InitializeCriticalSection(&cs_);
		}
		void Lock()
		{
			// Enter the critical section
			EnterCriticalSection(&cs_);	
		}
		void Unlock()
		{
			// Leave the critical section
			LeaveCriticalSection(&cs_);
		}
		~Mutex(void)
		{
			//Make sure the CRITICAL SECTION is unlocked
			Unlock();
			// Release system object when deleted
			DeleteCriticalSection(&cs_);
		}
	};

}