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

		ThreadPool(void);						//Default Constructor

		ThreadPool(const int &PoolSize);		//Constructor that specifys the pool size

		~ThreadPool(void);						//deconstructor


		//Run methods that can take various types n objects
		template <class C, class P>
		void Schedule(C* pClass, void (C::*pfFunc)(P), P p );

		template <class C>
		void Schedule(C* pClass, void (C::*pfFunc)(void));

		template <class C, class P>
		void Schedule(C* pClass, P p);

		template <class C>
		void Schedule(C* pClass);
		
		void Schedule(RunnableBase* pClass);	//schedules a job of type RunnableBase

		void Wait();							//waits until all jobs are complete

	private:
		static const int DEFAULT_POOL_SIZE = 10;//default size 
		int poolSize_;							//the size of this pool

		std::queue<RunnableBase*> jobQueue_;	//job queue
		std::vector<PThread> threads_;			//threads within this pool
		Thread poolThread_;						//thread the pool runs on
		std::vector<HANDLE> runningEvents_;		//running events of the threads

		Mutex jobQueueMutex_;					//mutex used for jobqueue container 
		HANDLE hJobAddedEvt_;					//job added event
		HANDLE hAllJobsFinishedEvt_;			//all jobs finished event

		void init(const int &PoolSize);			//sets up the pool object
		void CheckSchedule(void);				//method that polls the job queue
		void RunNextJob(int &thread);			//Run the next job on a certain thread

	};

	ThreadPool::ThreadPool(const int &poolSize)
	{
		init(poolSize);	//initialises the pool
	}

	ThreadPool::ThreadPool(void)
	{
		init(DEFAULT_POOL_SIZE); //initialises the pool
	}

	void ThreadPool::init(const int &poolSize)
	{
		//Create job added event
		hJobAddedEvt_ = CreateEvent( 
					NULL,               // default security attributes
					TRUE,               // manual-reset event
					FALSE,              // initial state is nonsignaled
					NULL  // object name
					); 

		//Create All jobs finished added event
		hAllJobsFinishedEvt_ = CreateEvent( 
					NULL,               // default security attributes
					TRUE,               // manual-reset event
					TRUE,              // initial state is signaled
					NULL  // object name
					); 
		poolSize_ = poolSize;

		//reserve space in vectors
		threads_.reserve(poolSize_);
		runningEvents_.reserve(poolSize_);

		//loop though until poolsize
		for(int i(0); i < poolSize_; ++i)
		{
			//Create a Thread
			PThread t;
			//Push it on to the vector
			threads_.push_back(t);
			
			//Push the run event on to the vector
			runningEvents_.push_back(threads_[i].StoppedEventHandle());

		}
		//the pool has its own thread that it runs on
		poolThread_.Run(this, &ThreadPool::CheckSchedule);
	
	}

	ThreadPool::~ThreadPool(void)
	{
		//close open handles
		CloseHandle(hJobAddedEvt_);
		CloseHandle(hAllJobsFinishedEvt_);
	}

	
	template <class C, class P>
	void ThreadPool::Schedule(C* pClass, void (C::*pfFunc)(P), P p )
	{
		return Schedule((RunnableBase*)
				new Runnable<C, P>(pClass, pfFunc, p)
				);
	}

	template <class C>
	void ThreadPool::Schedule(C* pClass, void (C::*pfFunc)(void))
	{
		return Schedule((RunnableBase*)
				new Runnable<C>(pClass, pfFunc)
				);
	}
	
	template <class C, class P>
	void ThreadPool::Schedule(C* pClass, P p)
	{
		return Schedule((RunnableBase*)
				new Runnable<C, P>(pClass, p)
		);
	}

	template <class C>
	void ThreadPool::Schedule(C* pClass)
	{
		return Schedule((RunnableBase*)
			new Runnable<C>(pClass)
		);
	}

	void ThreadPool::Schedule(RunnableBase* pFunctor)
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
					//run next job on free thread
					RunNextJob(i);

					//jump out of loop
					break;
				}
			}
		}
	}
	void ThreadPool::RunNextJob(int &thread)
	{
		//lock mutex
		jobQueueMutex_.Lock();

		//Run the next queued
		threads_[thread].Run(jobQueue_.front());
		//pop it from the front
		jobQueue_.pop();
		//if no items left in the quee reset the event
		if(jobQueue_.size()==0)
		{
			//reset job added event
			ResetEvent(hJobAddedEvt_);

			DWORD allRunning = 
				WaitForMultipleObjects(
					static_cast<DWORD>(runningEvents_.size()),// number of objects in array
					&runningEvents_[0],     // array of objects
					TRUE,       // wait for any object
					INFINITE);       // wait for ever

			if(allRunning==0)
				//set the all jobs finished event
				SetEvent(hAllJobsFinishedEvt_);
		}
		//unlock mutex
		jobQueueMutex_.Unlock();
	}
}