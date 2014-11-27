/********************************************************************
	created:  2014/10/08	13:53:36
	filename: InterruptHandlingPolicy.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_THREADINGUTILS__INTERRUPTHANDLINGPOLICY_H__
#define __HEADER_GUARD_THREADINGUTILS__INTERRUPTHANDLINGPOLICY_H__

#include <threadingUtils/InterruptiblePolicy.h>

namespace threadingUtils
{
	//! Wszystkie polityki obs�ugi przerwania w�tku realizowane s� w ramach przerywanego w�tku
	//! Musz� mie� dost�p do mechanizm�w zarz�dzajacych przerywaniami


	//! Polityka braku obs�ugi przerwania - ponownie rzuca ten sam wyj�tek
	struct NoInterruptHandlingPolicy
	{
		template<typename InterruptPolicy>
		void handle(const ThreadInterruptedException & e)
		{
			throw e;
		}
	};

	//! Polityka gasz�ca przerwanie
	struct ConsumeInterruptHandlingPolicy
	{
		template<typename InterruptPolicy>
		void handle(const ThreadInterruptedException & e)
		{

		}
	};

	//! Polityka gasz�ca i resetuj�ca przerwanie, aby m�c ponownie przerwa�
	struct ResetInterruptHandlingPolicy
	{
		template<typename InterruptPolicy>
		void handle(const ThreadInterruptedException & e)
		{
			InterruptPolicy::resetInterruption();
		}
	};
}

#endif	// __HEADER_GUARD_THREADINGUTILS__INTERRUPTHANDLINGPOLICY_H__