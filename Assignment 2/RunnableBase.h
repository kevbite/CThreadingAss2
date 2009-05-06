#pragma once
namespace kevsoft {
	/* Abstract class used for storing jobs on the thread */
	class RunnableBase {
	public:
		virtual void operator ()(void) = 0;
		virtual RunnableBase* Clone(void) = 0;

	};

}