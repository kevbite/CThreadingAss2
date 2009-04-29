
// Assignment 2.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <string>
#include "Thread.h"
#include <conio.h>


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
	}
	void operator ()(void)
	{
		std::cout << "Void Stuff";
	}
};

int _tmain(int argc, _TCHAR* argv[])
{
	A *a = new A();
	kevsoft::Thread<A, int> *thread1 = new kevsoft::Thread<A, int>();
	kevsoft::Thread<A, bool> *thread2 = new kevsoft::Thread<A, bool>();
	kevsoft::Thread<A, std::string> *thread3 = new kevsoft::Thread<A, std::string>();
	kevsoft::Thread<A, std::string> *thread3cpy = new kevsoft::Thread<A, std::string>(*thread3);
	kevsoft::Thread<A, void> *thread4 = new kevsoft::Thread<A, void>();

	thread1->Run(a, &A::operator (), 0);
	thread2->Run(a, true);
	thread3->Run(a, "Hello");
	thread4->Run(a);
	thread4->Run(a);
	thread4->Run(a);


	thread1->Wait();
	thread2->Wait();
	thread3->Wait();
	thread4->Wait();
	
	std::cin.get();
	thread1->Run(a, &A::operator (), 0);
	
	
	std::cin.get();

	delete thread1;
	delete thread2;
	delete thread3;
	//delete thread4;

	return 0;
}
