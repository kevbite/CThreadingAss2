#pragma once
#include "RunnableBase.h"

namespace kevsoft {

	/* Runnable object for the Thread class
		-Enables so that the thread class can run any type of object
	*/
	template<class T, class P = void>
	class Runnable : public RunnableBase {
	public:
		//Function pointer
		typedef void (T::*FuncPtr)(P);
		
		//Constructor for a class object and arguments
		Runnable(T* functor, P args)
		{
			class_ = functor;
			args_ = args;
			pfFunc_  = &T::operator ();
		}
		//Constructor for a class object + method and arguments
		Runnable(T* functor, FuncPtr funcPtr, P args)
		{
			class_ = functor;
			args_ = args;
			pfFunc_ = funcPtr;
		}
		//Clone method
		RunnableBase* Clone(void)
		{
			//returns a clone of this object
			return new Runnable<T,P>(
				&T(*class_),
				FuncPtr(pfFunc_),
				P(args_)
				);
		}
		void operator ()(void)
		{
			//calls the specified method within the given class
			(class_->*pfFunc_)(args_);
		}

	private:
		T* class_;			//Class
		FuncPtr pfFunc_;	//Function to call
		P args_;			//Arguments to pass

	};

	/*	Specialization of Runnable Templated object that takes no parameters
	*/
	template<class T>
	class Runnable<T, void> : public RunnableBase {
	public:

		typedef void (T::*FuncPtr)(void);

		//Constructor for Class object
		Runnable(T* functor)
		{
			class_ = functor;
			pfFunc_ = &T::operator ();
		}
		//Constructor for class and method
		Runnable(T* functor,FuncPtr funcPtr)
		{
			class_ = functor;
			pfFunc_ = funcPtr;
		}
		//Clones this object
		RunnableBase* Clone(void)
		{
			// clone this object
			RunnableBase* clone =  new Runnable<T> (
				&T(*class_),
				FuncPtr(pfFunc_)
				);

			//Return clone
			return clone;
		}

		void operator ()(void)
		{
			//calls the specified classes method
			(class_->*pfFunc_)();
		}

	private:
		T* class_;			//Class
		FuncPtr pfFunc_;	//Function to call

	};
}