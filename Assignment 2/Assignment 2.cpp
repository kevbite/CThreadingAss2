
// Assignment 2.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <string>
#include "Thread.h"
#include <conio.h>
#include "windows.h" 

#include "crtdbg.h"

#include "ThreadPool.h"

class A
{
private:
	std::string string_;
public:
	void DoSomeThing(void *)
	{
		std::cout << "Done it";
	}
	void operator ()(int)
	{
		std::cout << "int Done it";
	}
	void operator ()(bool)
	{
		std::cout << "bool Done it";
	}
	void operator ()(std::string s)
	{
		std::cout << s;
		Sleep(1000);
	}
	void operator ()(void)
	{
		std::cout << "Void Stuff";
	}
};
class SleepFunctor{
public:
	void operator()(int time)
	{
		std::cout << "Sleep Called:" << time << std::endl;
		Sleep(time);
	}
	void operator ()(void)
	{
		std::cout << "Sleep Called:" << 1000 << std::endl;
		Sleep(1000);
	}
	void sleep(void)
	{
		std::cout << "Sleep func Called:" << 1000 << std::endl;
		Sleep(1000);
	}
	void sleep(int time)
	{
		std::cout << "Sleep func Called:" << time << std::endl;
		Sleep(time);
	}
};
class PrimeNumber
{
public:
	void operator ()(int num)
	{
		found_.clear();

		bool isPrime=true;
		for ( int i = 0; i <= num; i++)
		{
			for ( int j = 2; j <= num; j++)
			{
				if ( i!=j && i % j == 0 )
				{
				  isPrime=false;
				  break;
				}
			}
			if (isPrime)
			{
				found_.push_back(i);
			}
			isPrime=true;
		}
	}
	std::vector<int> PrimesFound() const
	{
		return found_;
	}
private:
	std::vector<int> found_;
};


class OutputData
{
public:

	template<class T>
	void operator ()(T container)
	{
		for (T::iterator it=container.begin() ; it < container.end(); it++ )
		{	
			if(it==container.begin())
				std::cout << *it;
			else
				std::cout << ", " << *it;

		}
	}
};

void TestThreadObject()
{
	std::cout << "____Testing Thread Object____" << std::endl;
	//Create thread object
	kevsoft::Thread thread1;
	//Set thread running to find prime numbers between 1 and 100
	std::cout << "Start running Prime numbers 1 - 1000" << std::endl;
	PrimeNumber workPrimes;//functor for the thread
	thread1.Run(&workPrimes, 1000);//run the functor on the thread passing an int
	Sleep(1);//sleep this thread so we get a few results
	thread1.Suspend();//suppend the thread

	//output the data we have so far
	std::cout << "Thread Suspended" << std::endl;
	std::cout << "Prime Numbers so far: ";
	OutputData()(workPrimes.PrimesFound());

	//resume the thread
	thread1.Resume();
	std::cout << std::endl << "Thread Resumed" << std::endl;

	//wait for the thread to end
	thread1.Wait();
	std::cout << "Thread Wait Exited" << std::endl;

	//output the prime numbers found
	std::cout << "Prime Numbers found: ";
	OutputData()(workPrimes.PrimesFound());

	std::cout << std::endl;
}
void TestThreadPoolObject()
{
	std::cout << "____Testing Thread Pool____" << std::endl;

	//Test pool of size 1
	std::cout << "Testing pool of size 1" << std::endl;
	kevsoft::ThreadPool pool1(1);

	//Schedule a Sleep functor with no pramaters
	pool1.Schedule(&SleepFunctor());
	//Schedule a Sleep functor with int pramaters
	pool1.Schedule(&SleepFunctor(), 3000);
	//Schedule a Sleep object to run the sleep method
	pool1.Schedule(&SleepFunctor(), &SleepFunctor::sleep);
	//Schedule a Sleep object to run the sleep method with int ppramaters
	pool1.Schedule(&SleepFunctor(), &SleepFunctor::sleep, 3000);

	std::cout << "Waiting for pool to finish" << std::endl;
	pool1.Wait();//wait for pool to finish its schedule
	std::cout << "pool finished" << std::endl << std::endl;


	//Test a pool of size 4
	std::cout << "Testing pool of size 4" << std::endl;
	kevsoft::ThreadPool pool2(4);

	//Schedule a Sleep functor with no pramaters
	pool2.Schedule(&SleepFunctor());
	//Schedule a Sleep functor with int pramaters
	pool2.Schedule(&SleepFunctor(), 3000);
	//Schedule a Sleep object to run the sleep method
	pool2.Schedule(&SleepFunctor(), &SleepFunctor::sleep);
	//Schedule a Sleep object to run the sleep method with int ppramaters
	pool2.Schedule(&SleepFunctor(), &SleepFunctor::sleep, 3000);

	std::cout << "Waiting for pool to finish" << std::endl;
	pool2.Wait();//wait for pool to finish its schedule
	std::cout << "pool finished" << std::endl;


}
int _tmain(int argc, _TCHAR* argv[])
{
	//Test the thread object
	TestThreadObject();

	//Test the thread pool object
	TestThreadPoolObject();
	
	_CrtDumpMemoryLeaks();

	std::cin.get();
	return 0;
}


