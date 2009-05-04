#pragma once
#include <queue>
#include <vector>
#include "PThread.h"
#include "windows.h"

namespace kevsoft{

	class ThreadPool
	{
	public:

		ThreadPool(void);

		ThreadPool(const int &PoolSize);

		void init(const int &PoolSize);

		~ThreadPool(void);

		template <class C, class P>
		void Schedule(C* pClass, void (C::*pfFunc)(P), P p );

		template <class C>
		void Schedule(C* pClass, void (C::*pfFunc)(void));

		template <class C, class P>
		void Schedule(C* pClass, P p);

		template <class C>
		void Schedule(C* pClass);
		
		void Schedule(ThreadFunctorBase* pClass);


	private:
		static const int DEFAULT_POOL_SIZE = 10;
		std::queue<ThreadFunctorBase*> threadQueue_;
		std::vector<PThread> threads_;
		Thread poolThread_;
		std::vector<HANDLE> runningEvents_;

		void CheckSchedule(void);

	};

	ThreadPool::ThreadPool(const int &poolSize)
	{
		init(poolSize);
	}

	ThreadPool::ThreadPool(void)
	{
		init(DEFAULT_POOL_SIZE);
	}

	void ThreadPool::init(const int &poolSize)
	{
		//reserve space in vectors
		threads_.reserve(poolSize);
		runningEvents_.reserve(poolSize);

		//loop though until poolsize
		
		{
			//Create a Pool Thread
			PThread t;
			//Push it on to the vector
			threads_.push_back(t);
			//Push the run event on to the vector
			runningEvents_.push_back(t.RunningEventHandle());
		}
		//the pool has its own thread that it runs on
		poolThread_.Run(this, &ThreadPool::CheckSchedule);
	
	}

	ThreadPool::~ThreadPool(void)
	{
	}

	
	template <class C, class P>
	void ThreadPool::Schedule(C* pClass, void (C::*pfFunc)(P), P p )
	{
		return Schedule((ThreadFunctorBase*)
				new ThreadFunctor<C, P>(pClass, pfFunc, p)
				);
	}

	template <class C>
	void ThreadPool::Schedule(C* pClass, void (C::*pfFunc)(void))
	{
		return Schedule((ThreadFunctorBase*)
				new ThreadFunctor<C>(pClass, pfFunc)
				);
	}
	
	template <class C, class P>
	void ThreadPool::Schedule(C* pClass, P p)
	{
		return Schedule((ThreadFunctorBase*)
				new ThreadFunctor<C, P>(pClass, p)
		);
	}

	template <class C>
	void ThreadPool::Schedule(C* pClass)
	{
		return Schedule((ThreadFunctorBase*)
			new ThreadFunctor<C>(pClass)
		);
	}

	void ThreadPool::Schedule(ThreadFunctorBase* pFunctor)
	{
		//add to the queue
		threadQueue_.push(pFunctor);
	}

	void ThreadPool::CheckSchedule(void)
	{
		while(true)
		{
			DWORD dwEvent = 
				WaitForMultipleObjects( 
					static_cast<DWORD>(runningEvents_.size()),// number of objects in array
					&runningEvents_[0],     // array of objects
					FALSE,       // wait for any object
					INFINITE);       // wait for ever

			//while there is nothing in the queue will will do nothing
			while(threadQueue_.size()==0);

			//loop though each event to see which it was
			for(int i(0); i < runningEvents_.size(); ++i)
			{
				if(dwEvent == (WAIT_OBJECT_0 + i))
				{
					//Run the next queued
					threads_[i].Run(threadQueue_.front());
					//pop it from the top
					threadQueue_.pop();
					//jump out of loop
					break;
				}
			}
		}
	}
}