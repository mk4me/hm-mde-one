/********************************************************************
	created:  2014/10/02	09:15:47
	filename: HierarchicalMutex.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_THREADINGUTILS__HIERARCHICALMUTEX_H__
#define __HEADER_GUARD_THREADINGUTILS__HIERARCHICALMUTEX_H__

#include <limits>
#include <stdexcept>
#include <threadingUtils/Macros.h>

namespace threadingUtils
{
	//! Typ mutexa hierarchicznego, gdzie kontroluje si� hierarchi� lokowania
	//! \tparam MutexType Typ mutexa u�ywanego w hierarchii
	template<typename MutexType>
	class HierarchicalMutex
	{
	public:
		//! Typ opisuj�cy poziom blokady
		typedef unsigned long HierarchyLevelType;

	private:
		//! Wewn�trzny mutex
		MutexType mutex;
		//! Aktualna warto�� hierarchi mutex�w
		const HierarchyLevelType hierarchyValue;
		//! Poprzednia warto�� hierarchi mutex�w
		HierarchyLevelType previousHierarchyValue;
		//! Warto�� hierarchi mutex�w tego w�tku
		static thread_local HierarchyLevelType threadHierarchyValue;

	private:

		//! Metoda sprawdza czy nie pogwa�cono zasad dla hierarchi mutex�w
		void checkForHierarchyViolation()
		{
			if (threadHierarchyValue <= hierarchyValue)
			{
				throw std::logic_error("mutex hierarchy violated");
			}
		}

		//! Metoda aktualizuje informacje o aktualnej hierarchi mutex�w
		void updateHierarchyValue()
		{
			previousHierarchyValue = threadHierarchyValue;
			threadHierarchyValue = hierarchyValue;
		}

	public:
		//! Konstruktor
		//! \param Warto�� hierarchii mutexa dla aktualnego w�tku
		explicit HierarchicalMutex(const HierarchyLevelType value) : hierarchyValue(value), previousHierarchyValue(0) {}
		//! Destruktor
		~HierarchicalMutex() {}
		//! Lock mutexa
		void lock()
		{
			checkForHierarchyViolation();
			mutex.lock();
			updateHierarchyValue();
		}
		//! Unlock mutexa
		void unlock()
		{
			threadHierarchyValue = previousHierarchyValue;
			mutex.unlock();
		}
		//! Pr�ba lokowania mutexa
		//! \return Prawda je�li uda�o si� zrobi� lock na mutexie
		const bool try_lock()
		{
			checkForHierarchyViolation();
			if (!mutex.try_lock())
				return false;
			updateHierarchyValue();
			return true;
		}
	};

	//! Inicjalizacja lokalnych danych w�tku na potrzeby obs�ugi mutex�w hierarchicznych
	template<typename MutexType>
	thread_local HierarchyLevelType HierarchicalMutex<MutexType>::threadHierarchyValue = std::numeric_limits<HierarchyLevelType>::max();
}

#endif	// __HEADER_GUARD_THREADINGUTILS__HIERARCHICALMUTEX_H__