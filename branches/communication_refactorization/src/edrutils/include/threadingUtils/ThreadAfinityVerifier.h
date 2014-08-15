/********************************************************************
	created:  2014/08/14	15:24:35
	filename: ThreadAfinityVerifier.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_THREADINGUTILS__THREADAFINITYVERIFIER_H__
#define __HEADER_GUARD_THREADINGUTILS__THREADAFINITYVERIFIER_H__

#include <threadingUtils/Export.h>
#include <utils/SmartPtr.h>

namespace threadingUtils
{
	class THREADINGUTILS_EXPORT ThreadAfinityVerifier
	{
		class ThreadAfinityVerifierImpl;

	protected:
		//! Domy�lny konstruktor
		ThreadAfinityVerifier();
		//! \return Zwraca prawd� je�li aktualny w�tek to ten sam dla kt�ego stworzono dany obiekt
		const bool verifyCurrentThreadAfinity() const;

	public:
		//! Destruktor wirtualny
		virtual ~ThreadAfinityVerifier();

	private:
		//! Implementacja
		utils::shared_ptr<ThreadAfinityVerifierImpl> impl;
	};
}

#endif	// __HEADER_GUARD_THREADINGUTILS__THREADAFINITYVERIFIER_H__