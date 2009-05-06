#pragma once
#include <queue>
#include <vector>
#include "PThread.h"
#include "windows.h"
#include "Mutex.h"

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

		void Wait();

	private:
		static const int DEFAULT_POOL_SIZE = 10;
		std::queue<ThreadFunctorBase*> jobQueue_;
		std::vector<PThread> threads_;
		Thread poolThread_;
		std::vector<HANDLE> runningEvents_;

		Mutex jobQueueMutex_;
		HANDLE hJobAddedEvt_;
		HANDLE hAllJobsFinishedEvt_;
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
		//Create job added event
		hJobAddedEvt_ = CreateEvent( 
					NULL,               // default security attributes
					TRUE,               // manual-reset event
					FALSE,              // initial state is nonsignaled
					TEXT("JobAddedEvt")  // object name
					); 

		//Create All jobs finished added event
		hAllJobsFinishedEvt_ = CreateEvent( 
					NULL,               // default security attributes
					TRUE,               // manual-reset event
					TRUE,              // initial state is signaled
					TEXT("JobsFinishedEvt")  // object name
					); 
		//reserve space in vectors
		threads_.reserve(poolSize);
		runningEvents_.reserve(poolSize);

		//loop though until poolsize
		for(int i(0); i < poolSize; ++i)
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
		//lock the mutex
		jobQueueMutex_.Lock();
		//add to the queue
		jobQueue_.push(pFunctor);
		//update the queue event
		SetEvent(hJobAddedEvt_);
		//reset the All Jobs Finished Event
		ResetEvent(hAllJobsFinishedEvt_);
		//unlock mutex
		jobQueueMutex_.Unlock();
	}

	void ThreadPool::Wait()
	{
		//wait until all jobs are finished
		WaitForSingleObject(hAllJobsFinishedEvt_ ,INFINITE);
	}
	void ThreadPool::CheckSchedule(void)
	{
		while(true)
		{
			//wait for a thread object to become free (using events)
			DWORD dwEvent = 
				WaitForMultipleObjects( 
					static_cast<DWORD>(runningEvents_.size()),// number of objects in array
					&runningEvents_[0],     // array of objects
					FALSE,       // wait for any object
					INFINITE);       // wait for ever

			//wait for job added event
			WaitForSingleObject(hJobAddedEvt_,INFINITE);

			//loop though each event to see which it was
			for(int i(0); i < (int)runningEvents_.size(); ++i)
			{
				if(dwEvent == (WAIT_OBJECT_0 + i))
				{
					//lock mutex
					jobQueueMutex_.Lock();

					//Run the next queued
					threads_[i].Run(jobQueue_.front());
					//pop it from the front
					jobQueue_.pop();
					//if no items left in the quee reset the event
					if(jobQueue_.size()==0)
					{
						//reset job added event
						ResetEvent(hJobAddedEvt_);
						//set the all jobs finished event
						SetEvent(hAllJobsFinishedEvt_);
					}
					//unlock mutex
					jobQueueMutex_.Unlock();

					//jump out of loop
					break;
				}
			}
		}
	}
}