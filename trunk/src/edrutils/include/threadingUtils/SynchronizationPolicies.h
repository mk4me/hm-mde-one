/********************************************************************
created:  2011/06/14
created:  14:6:2011   8:48
filename: SynchronizationPolicies.h
author:   Mateusz Janiak

purpose:
*********************************************************************/
#ifndef HEADER_GUARD_THREADINGUTILS__SYNCHRONIZATIONPOLICIES_H__
#define HEADER_GUARD_THREADINGUTILS__SYNCHRONIZATIONPOLICIES_H__

#include <threadingUtils/Export.h>
#include <limits>
#include <utils/SmartPtr.h>

namespace threadingUtils {
	//! Forward declaration
	class ConditionVariable;

	//! Klasa bazowa polityk sycnhronizacji - nic nie robi
	class THREADINGUTILS_EXPORT LockingPolicy
	{
	public:
		//! Domyślny konstruktor
		LockingPolicy();
		//! Destruktor wirtualny
		virtual ~LockingPolicy();
		//! Blokuje obiekt
		virtual void lock();
		//! Odblokowuje obiekt
		virtual void unlock();
		//! \return Czy udało się zablokować obiekt
		virtual const bool tryLock();
	private:
		//! Konstruktor kopiujący
		LockingPolicy(const LockingPolicy &);
		//! Operator przypisania
		LockingPolicy & operator=(const LockingPolicy &);
	};

	//! Pusta polityka synchronizacji - nic nie robi
	typedef LockingPolicy NoSyncPolicy;

	//! Polityka synchronizacji rekurencyjnej
	class THREADINGUTILS_EXPORT RecursiveSyncPolicy : public LockingPolicy
	{
		friend class ConditionVariable;

	private:
		//! Forward declaration
		class RecursiveSyncPolicyImpl;

	public:
		RecursiveSyncPolicy();
		virtual ~RecursiveSyncPolicy();

		virtual void lock();
		virtual void unlock();
		virtual const bool tryLock();

	private:
		RecursiveSyncPolicy(const RecursiveSyncPolicy &);
		RecursiveSyncPolicy & operator=(const RecursiveSyncPolicy &);

	private:
		//! Implementacja funkcjonalności
		utils::shared_ptr<RecursiveSyncPolicyImpl> impl;
	};

	//! Polityka ścisłej synchronizacji
	class THREADINGUTILS_EXPORT StrictSyncPolicy : public LockingPolicy
	{
		friend class ConditionVariable;

	private:
		//! Forward declaration
		class StrictSyncPolicyImpl;

	public:

		StrictSyncPolicy();
		virtual ~StrictSyncPolicy();

		virtual void lock();
		virtual void unlock();
		virtual const bool tryLock();
	private:
		StrictSyncPolicy(const StrictSyncPolicy &);
		StrictSyncPolicy & operator=(const StrictSyncPolicy &);

	private:
		//! Implementacja funkcjonalności
		utils::shared_ptr<StrictSyncPolicyImpl> impl;
	};

	class THREADINGUTILS_EXPORT ConditionVariable
	{
		//! Forward declaration
		class ConditionVariableImpl;

	public:
		//! Domyślny konstruktor
		ConditionVariable();
		//! Destruktor
		~ConditionVariable();

		//! \param lock Obiekt synchronizujący na którym czekamy
		//! \param time Czas oczekiwania [ms], domyslnie max
		void wait(LockingPolicy * lock, const unsigned long time = ULONG_MAX);
		//! Wzbudza jednego z oczekujących
		void wakeOne();
		//! Wzbudza wszystkich oczekujących
		void wakeAll();

	private:
		ConditionVariable(const ConditionVariable &);
		ConditionVariable & operator=(const ConditionVariable &);

	private:
		//! Obiekt realizujący funkcjonalność
		utils::shared_ptr<ConditionVariableImpl> impl;
	};

	class THREADINGUTILS_EXPORT EmptyScopedLock
	{
	public:
		template<class SyncPolicy>
		EmptyScopedLock(SyncPolicy & mux) {}

		~EmptyScopedLock() {}
	};

	template<class SyncPolicy>
	class ScopedLock
	{
	public:
		ScopedLock(SyncPolicy & mux) : mux(mux)
		{
			mux.lock();
		}

		~ScopedLock()
		{
			mux.unlock();
		}

	private:
		LockingPolicy & mux;
	};
}

#endif  //  HEADER_GUARD_THREADINGUTILS__SYNCHRONIZATIONPOLICIES_H__
