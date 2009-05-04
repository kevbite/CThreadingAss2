#pragma once
namespace kevsoft {

	class ThreadFunctorBase {
	public:
		virtual void operator ()(void) = 0;
		virtual ThreadFunctorBase* Clone(void) = 0;

	};

}