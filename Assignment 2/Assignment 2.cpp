
// Assignment 2.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <string>
#include "Thread.h"
#include <conio.h>
#include "windows.h" 

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
						std::cout <<"Prime:"<< i << std::endl;
					}
					isPrime=true;
			}
	}
};
void TestThreadPoolObject()
{
	std::cout << "____Testing Thread Pool____" << std::endl;
	kevsoft::ThreadPool pool(3);

	//Schedule a Sleep functor with no pramaters
	pool.Schedule(&SleepFunctor());
	//Schedule a Sleep functor with int pramaters
	pool.Schedule(&SleepFunctor(), 3000);
	//Schedule a Sleep object to run the sleep method
	pool.Schedule(&SleepFunctor(), &SleepFunctor::sleep);
	//Schedule a Sleep object to run the sleep method with int ppramaters
	pool.Schedule(&SleepFunctor(), &SleepFunctor::sleep, 3000);

	pool.Wait();
}
int _tmain(int argc, _TCHAR* argv[])
{
	/*A *a = new A();
	kevsoft::Thread<A, std::string> *thread1 = new kevsoft::Thread<A, std::string>();
	kevsoft::Thread<A, std::string> *thread2 = new kevsoft::Thread<A, std::string>();
	kevsoft::Thread<A, std::string> *thread3 = new kevsoft::Thread<A, std::string>();
	

	thread1->Run(a, "1");
	thread1->Wait();
	thread2->Run(a, "2");
	thread2->Wait();
	thread3->Run(a, "3");
	thread3->Wait();

	delete thread1;
	delete thread2;
	delete thread3;*/

	//kevsoft::Thread thread1;
	//kevsoft::Thread thread2;

	//thread1.Run(&SleepFunctor());
	//std::cout << "Start 1... running: " << thread1.isRunning() << std::endl;
	//thread1.Wait();
	//thread1.isRunning();
	//std::cout << "Done 1... running: " << thread1.isRunning() << std::endl;

	//thread2.Run(&SleepFunctor());
	//thread2.Wait();
	//std::cout << "Done 2..." << std::endl;

	//thread1.Run(&SleepFunctor());
	//thread1.Wait();
	//std::cout << "Done 1..." << std::endl;

	//thread2.Run(&SleepFunctor(), &SleepFunctor::sleep);
	//thread2.Wait();
	//std::cout << "Done 2..." << std::endl;

	//thread1.Run(&SleepFunctor(), 1000);
	//thread1.Wait();
	//std::cout << "Done 1 with args..." << std::endl;

	//thread2.Run(&SleepFunctor(), &SleepFunctor::sleep, 1000);
	//thread2.Wait();
	//std::cout << "Done 2 with args..." << std::endl;

	//std::cout << "Run std::string arg on A" << std::endl;
	//thread1.Run(&A(), std::string("Hello123"));
	//std::cout << "Copying Thread1" << std::endl;
	//kevsoft::Thread thread1cpy(thread1);
	////thread1.isRunning();
	//thread1.Wait();
	//thread1cpy.Wait();

	//thread1.Run(&A(), std::string("Hello123"));
	
	kevsoft::Thread thread;
	thread.Wait();

	TestThreadPoolObject();
	//std::cin.get();
	return 0;
}


