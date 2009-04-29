#pragma once
#include <windows.h>
#include <process.h>
#include <math.h>
#include "Exceptions.h"

namespace kevsoft {
//C = Class
//P = Pramamter

template<class C, class P = void>
class Thread
{

public:
    typedef void (C::*FuncPtr)(P );
    
	Thread(void);

	~Thread(void);

	Thread(Thread<C,P>&);

	void Run(C* pClass, FuncPtr pfFunc, P p );

	void Run(C* pClass, FuncPtr pfFunc);

	void Run(C* pClass, P p);

	void Run(C* pClass);

	void Suspend();
 
	void Resume();

	void Terminate();

	void Wait();

	void CallThreadFunc();

	static unsigned int __stdcall funcCall(void* pArg) {

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

private:
	bool running_;

	FuncPtr pfFunc_;		//Function to call
	C *pClass_;				//The Functor/Class
	P pArgs_;				//Arguments to pass to function

    HANDLE handle_;			// Thread Handle
    unsigned tid_;			// Thread ID

};

template<class C>
class Thread<C,void>
{

public:
    typedef void (C::*FuncPtr)(void);
    
	Thread(void);

	~Thread(void);

	Thread(Thread<C>&);

	void Run(C* pClass, FuncPtr pfFunc);

	void Run(C* pClass);

	void Suspend();
 
	void Resume();

	void Terminate();

	void Wait();

	void CallThreadFunc();

	static unsigned int __stdcall funcCall(void* pArg) {

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

private:
	bool running_;

	FuncPtr pfFunc_;	//Function to call
	C *pClass_;			//Class

    HANDLE handle_;		//Thread Handle
    unsigned tid_;		// Thread ID

};


template<class C, class P>
Thread<C,P>::Thread()
: running_(false), pfFunc_(0), pClass_(0)
{
	handle_ = (HANDLE)_beginthreadex(
            0, // Security attributes
            0, // Stack size
			&funcCall, //func to call
            this, //arg to pass
            CREATE_SUSPENDED, //flags
            &tid_); //thread id
}

template<class C>
Thread<C,void>::Thread()
: running_(false), pfFunc_(0), pClass_(0)
{
	handle_ = (HANDLE)_beginthreadex(
            0, // Security attributes
            0, // Stack size
			&funcCall, //func to call
            this, //arg to pass
            CREATE_SUSPENDED, //flags
            &tid_); //thread id
}

template<class C, class P>
Thread<C,P>::~Thread()
{
	Terminate();
	CloseHandle (handle_);
}

template<class C>
Thread<C,void>::~Thread()
{
	Terminate();
	CloseHandle (handle_);
}

template<class C, class P>
Thread<C,P>::Thread(Thread<C,P>& thread) 
: running_(false), pfFunc_(0), pClass_(0)
{
	if(thread.pfFunc_!=0)
		pfFunc_ = thread.pfFunc_;
	if(thread.pClass_!=0)
		pClass_ = new C(*thread.pClass_);

	pArgs_ = P(thread.pArgs_);
	
	handle_ = (HANDLE)_beginthreadex(
            0, // Security attributes
            0, // Stack size
			&funcCall, //func to call
            this, //arg to pass
            CREATE_SUSPENDED, //flags
            &tid_); //thread id
}


template<class C, class P>
void Thread<C,P>::Run(C *pClass, FuncPtr pfFunc, P pArgs)
{
	//set the pointers
	pClass_ = pClass;
	pfFunc_ = pfFunc;
	pArgs_ = pArgs;

	//resume the thread
	ResumeThread (handle_);
}

template<class C, class P>
void Thread<C,P>::Run(C *pClass, P pArgs)
{
	//set the pointers
	pClass_ = pClass;
	pfFunc_ = &C::operator ();
	pArgs_ = pArgs;

	//resume the thread
	ResumeThread (handle_);
}

template <class C, class P>
void Thread<C,P>::Run(C* pClass, FuncPtr pfFunc)
{
	//set the pointers
	pClass_ = pClass;
	pfFunc_ = pfFunc;
	pArgs_ = 0;

	//resume the thread
	ResumeThread (handle_);
}

template <class C>
void Thread<C,void>::Run(C* pClass, FuncPtr pfFunc)
{
	//set the pointers
	pClass_ = pClass;
	pfFunc_ = pfFunc;

	//resume the thread
	ResumeThread (handle_);
}

template <class C, class P>
void Thread<C,P>::Run(C* pClass)
{
	//set the pointers
	pClass_ = pClass;
	pfFunc_ = &C::operator ();
	pArgs_ = pArgs;

	//resume the thread
	ResumeThread (handle_);
}

template <class C>
void Thread<C,void>::Run(C* pClass)
{
	//set the pointers
	pClass_ = pClass;
	pfFunc_ = &C::operator ();

	//resume the thread
	ResumeThread (handle_);
}

template<class C, class P>
void Thread<C,P>::Resume()
{
	ResumeThread (handle_);
}

template<class C>
void Thread<C,void>::Resume()
{
	if(running_) throw exceptions::AlreadyRunningException();

	running_ = true;
	ResumeThread (handle_);
}

template<class C, class P>
void Thread<C,P>::Suspend()
{
	if(running_) throw exceptions::AlreadyRunningException();

	running_ = true;
	SuspendThread(handle_);
}

template<class C>
void Thread<C,void>::Suspend()
{
	running_ = false;
	SuspendThread(handle_);
}

template<class C, class P>
void Thread<C,P>::Terminate()
{
	running_ = false;
	TerminateThread(handle_, 0);
}

template<class C>
void Thread<C,void>::Terminate()
{
	TerminateThread(handle_, 0);
}

template<class C, class P>
void Thread<C,P>::Wait()
{
	while(running_);
}

template<class C>
void Thread<C,void>::Wait()
{
	while(running_);
}

template<class C, class P>
void Thread<C,P>::CallThreadFunc()
{
	(pClass_->*pfFunc_)(pArgs_);
}

template<class C>
void Thread<C, void>::CallThreadFunc()
{
	(pClass_->*pfFunc_)();
}


}