#pragma once
#include <windows.h>
#include <process.h>
#include <math.h>

//C = Class

template<class C, class P = void>
class Thread
{

public:
    typedef void (C::*FuncPtr)(P );
    
	Thread(void);

	~Thread(void);

	void Run(C* pClass, FuncPtr pfFunc, P p );

	void Run(C* pClass, FuncPtr pfFunc);

	void Suspend();
 
	void Resume();

	void Terminate();

	void CallThreadFunc();

	static unsigned int __stdcall funcCall(void* pArg) {
		//cast to thread object
		Thread *pThread = (Thread*)pArg;
		//call thread func
		pThread->CallThreadFunc();
	return 0;
  }   

private:
	FuncPtr pfFunc_;
	C *pClass_;
	P pArgs_;


    HANDLE handle_;
    unsigned tid_;     // Thread ID

};
template<class C, class P>
Thread<C,P>::Thread()
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
	CloseHandle (handle_);
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
void Thread<C,P>::Resume()
{
	ResumeThread (handle_);
}
template<class C, class P>
void Thread<C,P>::CallThreadFunc()
{
	(pClass_->*pfFunc_)(pArgs_);
}