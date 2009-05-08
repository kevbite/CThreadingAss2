#pragma once
#include <windows.h>
#include <process.h>
#include "Exceptions.h"
#include "Runnable.h"

namespace kevsoft {
	/*
	Thread class enables quick access to threading 
	without directly using the windows API
	*/
	class Thread
	{

	public:
	   
		Thread(void);						//Default Constructor

		~Thread(void);						//Destructor

		Thread(const Thread&);				//Copy Constructor

		void init();						//initilise Thread

		//Run Methods to accept diffrent types of objects, functions and parameters
		template <class C, class P>			
		bool Run(C* pClass, void (C::*pfFunc)(P), P p );//Run method for a method inside an object
														//with paramters

		template <class C>
		bool Run(C* pClass, void (C::*pfFunc)(void));//Run method for a method inside an object

		template <class C, class P>
		bool Run(C* pClass, P p);			//Run method for a Functor with Paramters

		template <class C>
		bool Run(C* pClass);				//Run method for Functor

		bool Run();				//Run the Functor

		bool Suspend();						//Suppends the thread
	 
		bool Resume();						//Resumes a Suppended Thread

		bool Terminate();					//Terminates Thread

		void Wait() const;					//Waits for the Thread to finish processing

		bool isRunning() const;				//Checks if the Thread in currently running

		Thread& Thread::operator=(const Thread& t);// Assignment Operator

		RunnableBase* Job();				//Returns pointer to current job

	protected:

		RunnableBase* job_;					//The Runnable Object

		HANDLE handle_;						//Thread Handle
		unsigned tid_;						// Thread ID

		bool Run(RunnableBase* pClass);		//Runs a RunnableBase Object

		HANDLE hStoppedEvent_;
	private:

		void CallThreadFunc() const;		//Used for looping back the call from Win32
		static unsigned int __stdcall funcCall(void* pArg);


	};

	Thread::Thread()
		: job_(0)
	{
		init();
	}

	Thread::~Thread()
	{
		Terminate();
		CloseHandle (handle_);
		CloseHandle (hStoppedEvent_);

		//delete the current functor
		delete job_;
		job_ = 0;
	}

	void Thread::init()
	{
		hStoppedEvent_ = CreateEvent( 
			NULL,               // default security attributes
			TRUE,               // manual-reset event
			TRUE,              // initial state is signaled
			NULL  // object name
			); 

		handle_ = (HANDLE)_beginthreadex(
				0, // Security attributes
				0, // Stack size
				&funcCall, //func to call
				this, //arg to pass
				CREATE_SUSPENDED, //flags
				&tid_); //thread id
	}

	Thread::Thread(const Thread& thread)
		:job_(0)
	{
		//This will set our Thread and Running event up
		init();		
		//Check if the thread has a functor assigned
		if(thread.job_!=0)
			//Copy it to this Thread
			job_ = (thread.job_->Clone());	
		
		//if the thread were copying is running
		if(thread.isRunning())
			//run this thread
			Resume();

	}


	template <class C, class P>
	bool Thread::Run(C* pClass, void (C::*pfFunc)(P), P p )
	{
		//create a runnable ojbect and pass to the correct run method
		return Run((RunnableBase*)
				new Runnable<C, P>(pClass, pfFunc, p)
				);
	}

	template <class C>
	bool Thread::Run(C* pClass, void (C::*pfFunc)(void))
	{
		//create a runnable ojbect and pass to the correct run method
		return Run((RunnableBase*)
				new Runnable<C>(pClass, pfFunc)
				);
	}
	
	template <class C, class P>
	bool Thread::Run(C* pClass, P p)
	{
		//create a runnable ojbect and pass to the correct run method
		return Run((RunnableBase*)
				new Runnable<C, P>(pClass, p)
		);
	}

	template <class C>
	bool Thread::Run(C* pClass)
	{
		//create a runnable ojbect and pass to the correct run method
		return Run((RunnableBase*)
			new Runnable<C>(pClass)
		);
	}
	bool Thread::Run()
	{
		//if no functor is set
		if(job_==0) return false;
		
		return Resume();
	}

	bool Thread::Run(RunnableBase* pFunctor)
	{
		//delete the current functor
		delete job_;
		job_ = 0;

		//set the job_ of class
		job_ = pFunctor;

		//resume the thread
		return Resume();
	}

	bool Thread::Resume()
	{
		if(isRunning()) throw exceptions::AlreadyRunningException();
		//try resume the thread
		//Resume Thread return values:
		// -1 = Failed
		// previous suspend count = success
		bool result = (ResumeThread (handle_) != -1);

		//if thread was resumed
		if(result)
			//reset the event so we know the tread is running
			ResetEvent(hStoppedEvent_);

		//return success
		return result;
	}

	bool Thread::Suspend()
	{
		//check that the thread is running 
		if(!isRunning()) return false;
		//Set the running event 
		SetEvent(hStoppedEvent_);
		
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
		SetEvent(hStoppedEvent_);
		//get the result of the termination of thread
		bool result = (TerminateThread(handle_, 0) != 0);
		//return result
		return result;
	}

	void Thread::Wait() const
	{
		//wait for a running event
		WaitForSingleObject(hStoppedEvent_,INFINITE);   
	}

	bool Thread::isRunning() const
	{
		//do a wait for 0 ms, if 0 is returned object is signaled.
		bool result = WaitForSingleObject(hStoppedEvent_, 0) != 0;
		//return result
		return result;
	}

	void Thread::CallThreadFunc() const
	{
		//if there is a thread job object assigned
		if(job_!=0)
			//run the operations
			(job_->operator())();
	}

	Thread& Thread::operator=(const Thread& t) {
		if (this != &t) {  // make sure not same object
			Suspend();							//Suppend the thread
			delete job_;					// Delete old data

			job_ = t.job_->Clone();		//clone the job object
		}
		return *this;    // Return ref for multiple assignment
	}

	RunnableBase* Thread::Job()
	{
		return job_;//returns the job
	}

	/* Static method used for the Win32 Thread API */
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