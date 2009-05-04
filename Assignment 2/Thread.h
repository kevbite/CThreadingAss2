#pragma once
#include <windows.h>
#include <process.h>
#include "Exceptions.h"
#include "ThreadFunctor.h"

namespace kevsoft {

	class Thread
	{

	public:
	   
		Thread(void);

		~Thread(void);

		Thread(const Thread&);

		template <class C, class P>
		bool Run(C* pClass, void (C::*pfFunc)(P), P p );

		template <class C>
		bool Run(C* pClass, void (C::*pfFunc)(void));

		template <class C, class P>
		bool Run(C* pClass, P p);

		template <class C>
		bool Run(C* pClass);

		bool Suspend();
	 
		bool Resume();

		bool Terminate();

		void Wait() const;

		bool isRunning() const;


	private:
		ThreadFunctorBase* functor_;

		HANDLE hRunningEvent_;
		HANDLE handle_;		//Thread Handle
		unsigned tid_;		// Thread ID

		bool Run(ThreadFunctorBase* pClass);

		void CallThreadFunc() const;

		static unsigned int __stdcall funcCall(void* pArg);


	};

	Thread::Thread()
		: functor_(0)
	{
		hRunningEvent_ = CreateEvent( 
			NULL,               // default security attributes
			TRUE,               // manual-reset event
			FALSE,              // initial state is nonsignaled
			TEXT("RunningEvt")  // object name
			); 

		handle_ = (HANDLE)_beginthreadex(
				0, // Security attributes
				0, // Stack size
				&funcCall, //func to call
				this, //arg to pass
				CREATE_SUSPENDED, //flags
				&tid_); //thread id
	}

	Thread::~Thread()
	{
		Terminate();
		CloseHandle (handle_);

		delete functor_;
	}

	Thread::Thread(const Thread& thread)
	{
		//Call Default Constructor
		//This will set our Thread and Running event up
		Thread();

		functor_ = thread.functor_->Clone();	
		
		//if the thread were copying is running
		if(thread.isRunning())
			//run this thread
			Resume();

	}


	template <class C, class P>
	bool Thread::Run(C* pClass, void (C::*pfFunc)(P), P p )
	{
		return Run(
				new ThreadFunctor<C, P>(pClass, pfFunc, p)
				);
	}

	template <class C>
	bool Thread::Run(C* pClass, void (C::*pfFunc)(void))
	{
		return Run(
				new ThreadFunctor<C>(pClass, pfFunc)
				);
	}
	
	template <class C, class P>
	bool Thread::Run(C* pClass, P p)
	{
		return Run(
				new ThreadFunctor<C, P>(pClass, p)
		);
	}

	template <class C>
	bool Thread::Run(C* pClass)
	{
		return Run(
			new ThreadFunctor<C>(pClass)
		);
	}

	bool Thread::Run(ThreadFunctorBase* pFunctor)
	{
		//set the functor_ of class
		functor_ = pFunctor;

		//resume the thread
		return Resume();
	}

	bool Thread::Resume()
	{
		//if(running_) throw exceptions::AlreadyRunningException();
		//try resume the thread
		//Resume Thread return values:
		// -1 = Failed
		// previous suspend count = success
		bool result = (ResumeThread (handle_) != -1);

		//if thread was resumed
		if(result)
			//reset the event so we know the tread is running
			ResetEvent(hRunningEvent_);

		//return success
		return result;
	}

	bool Thread::Suspend()
	{
		//Set the running event 
		SetEvent(hRunningEvent_);
		
		//try resume the thread
		//Resume Thread return values:
		// -1 = Failed
		// previous suspend count = success
		bool result = (SuspendThread(handle_) != -1);

		//return success
		return result;
	}

	bool Thread::Terminate()
	{
		//set the running event
		SetEvent(hRunningEvent_);
		//get the result of the termination of thread
		bool result = (TerminateThread(handle_, 0) != 0);
		//return result
		return result;
	}

	void Thread::Wait() const
	{
		//wait for a running event
		WaitForSingleObject(hRunningEvent_,INFINITE);   
	}

	bool Thread::isRunning() const
	{
		//do a wait for 0 ms, if 0 is returned object is signaled.
		bool result = WaitForSingleObject(hRunningEvent_, 0) != 0;
		//return result
		return result;
	}

	void Thread::CallThreadFunc() const
	{
		//if there is a thread functor object assigned
		if(functor_!=0)
			//run the operations
			(functor_->operator())();
	}

	/* Static */
	unsigned int __stdcall Thread::funcCall(void* pArg) {

		while (true)//we always want to keep this thread running
		{
			//cast to thread object
			Thread *pThread = (Thread*)pArg;
			//call thread func
			pThread->CallThreadFunc();
			
			//suspend the thread
			pThread->Suspend();
		}

		return 0;
	}   


}