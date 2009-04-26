
// Assignment 2.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include "Thread.h"
#include <conio.h>

class A
{
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
};

int _tmain(int argc, _TCHAR* argv[])
{
	A *a = new A();
	Thread<A, int> *thread1 = new Thread<A, int>();
	Thread<A, bool> *thread2 = new Thread<A, bool>();

	thread1->Run(a, &A::operator (), 0);
	thread2->Run(a, &A::operator (), 0);

	while(!_kbhit());
	return 0;
}

