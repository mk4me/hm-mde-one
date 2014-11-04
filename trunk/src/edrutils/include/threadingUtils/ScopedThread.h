/********************************************************************
	created:  2014/10/02	10:08:53
	filename: ScopedThread.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_THREADINGUTILS__SCOPEDTHREAD_H__
#define __HEADER_GUARD_THREADINGUTILS__SCOPEDTHREAD_H__

#include <stdexcept>

namespace threadingUtils
{
	//! Klasa pomocnicza (RAII) przy czekaniu na w�tki, przejmuje w�tek
	//! \tparam Thread Typ w�tku
	template<class Thread>
	class ScopedThread
	{
	public:
		//! \param t_ W�tek kt�rego pilnujemy
		explicit ScopedThread(Thread t_) : t(std::move(t_))
		{
			if (t.joinable() == false){
				throw std::logic_error("Empty thread");
			}
		}

		//! Destruktor
		~ScopedThread()
		{
			t.join();
		}

		ScopedThread(ScopedThread const&) = delete;
		ScopedThread& operator=(ScopedThread const&) = delete;

	private:
		//! Obs�ugiwany w�tek
		Thread t;
	};
}

#endif	// __HEADER_GUARD_THREADINGUTILS__SCOPEDTHREAD_H__