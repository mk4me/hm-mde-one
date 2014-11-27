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
	//! Wszystkie polityki obs³ugi przerwania w¹tku realizowane s¹ w ramach przerywanego w¹tku
	//! Musz¹ mieæ dostêp do mechanizmów zarz¹dzajacych przerywaniami


	//! Polityka braku obs³ugi przerwania - ponownie rzuca ten sam wyj¹tek
	struct NoInterruptHandlingPolicy
	{
		template<typename InterruptPolicy>
		void handle(const ThreadInterruptedException & e)
		{
			throw e;
		}
	};

	//! Polityka gasz¹ca przerwanie
	struct ConsumeInterruptHandlingPolicy
	{
		template<typename InterruptPolicy>
		void handle(const ThreadInterruptedException & e)
		{

		}
	};

	//! Polityka gasz¹ca i resetuj¹ca przerwanie, aby móc ponownie przerwaæ
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