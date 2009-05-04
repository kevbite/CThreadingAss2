#pragma once
#include "ThreadFunctorBase.h"

namespace kevsoft {

	template<class T, class P = void>
	class ThreadFunctor : public ThreadFunctorBase {
	public:

		typedef void (T::*FuncPtr)(P);

		ThreadFunctor(T* functor, P args)
		{
			functor_ = functor;
			args_ = args;
			pfFunc_ = &T::operator ();
		}
		ThreadFunctor(T* functor, FuncPtr funcPtr, P args)
		{
			functor_ = functor;
			args_ = args;
			pfFunc_ = funcPtr;
		}
		ThreadFunctorBase* Clone(void)
		{
			return new ThreadFunctor<T,P>(
				&T(*functor_),
				FuncPtr(pfFunc_),
				P(args_)
				);
		}

		void operator ()(void)
		{
			(functor_->*pfFunc_)(args_);
		}

	private:
		T* functor_;		//Class
		FuncPtr pfFunc_;	//Function to call
		P args_;

	};

	template<class T>
	class ThreadFunctor<T, void> : public ThreadFunctorBase {
	public:

		typedef void (T::*FuncPtr)(void);

		ThreadFunctor(T* functor)
		{
			functor_ = functor;
			pfFunc_ = &T::operator ();
		}
		ThreadFunctor(T* functor,FuncPtr funcPtr)
		{
			functor_ = functor;
			pfFunc_ = funcPtr;
		}
		ThreadFunctorBase* Clone(void)
		{
			return new ThreadFunctor<T> (
				&T(*functor_),
				FuncPtr(pfFunc_)
				);
		}

		void operator ()(void)
		{
			(functor_->*pfFunc_)();
		}

	private:
		T* functor_;		//Class
		FuncPtr pfFunc_;	//Function to call

	};
}