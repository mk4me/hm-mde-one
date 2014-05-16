/********************************************************************
created:  2011/06/14
created:  14:6:2011   8:48
filename: SynchronizationPolicies.h
author:   Mateusz Janiak

purpose:
*********************************************************************/
#ifndef HEADER_GUARD_THREADINGUTILS__SYNCHRONIZATIONPOLICIES_H__
#define HEADER_GUARD_THREADINGUTILS__SYNCHRONIZATIONPOLICIES_H__

#include <limits>
#include <utils/SmartPtr.h>

namespace threadingUtils {
	//! Forward declaration
	class ConditionVariable;

	//! Klasa bazowa polityk sycnhronizacji - nic nie robi
	class LockingPolicy
	{
	public:
		//! Domy�lny konstruktor
		LockingPolicy();
		//! Destruktor wirtualny
		virtual ~LockingPolicy();
		//! Blokuje obiekt
		virtual void lock();
		//! Odblokowuje obiekt
		virtual void unlock();
		//! \return Czy uda�o si� zablokowa� obiekt
		virtual const bool tryLock();
	private:
		//! Konstruktor kopiuj�cy
		LockingPolicy(const LockingPolicy &);
		//! Operator przypisania
		LockingPolicy & operator=(const LockingPolicy &);
	};

	//! Pusta polityka synchronizacji - nic nie robi
	typedef LockingPolicy NoSyncPolicy;

	//! Polityka synchronizacji rekurencyjnej
	class RecursiveSyncPolicy : public LockingPolicy
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
		//! Implementacja funkcjonalno�ci
		utils::shared_ptr<RecursiveSyncPolicyImpl> impl;
	};

	//! Polityka �cis�ej synchronizacji
	class StrictSyncPolicy : public LockingPolicy
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
		//! Implementacja funkcjonalno�ci
		utils::shared_ptr<StrictSyncPolicyImpl> impl;
	};

	class ConditionVariable
	{
		//! Forward declaration
		class ConditionVariableImpl;

	public:
		//! Domy�lny konstruktor
		ConditionVariable();
		//! Destruktor
		~ConditionVariable();

		//! \param lock Obiekt synchronizuj�cy na kt�rym czekamy
		//! \param time Czas oczekiwania [ms], domyslnie max
		void wait(LockingPolicy * lock, const unsigned long time = ULONG_MAX);
		//! Wzbudza jednego z oczekuj�cych
		void wakeOne();
		//! Wzbudza wszystkich oczekuj�cych
		void wakeAll();

	private:
		ConditionVariable(const ConditionVariable &);
		ConditionVariable & operator=(const ConditionVariable &);

	private:
		//! Obiekt realizuj�cy funkcjonalno��
		utils::shared_ptr<ConditionVariableImpl> impl;
	};

	class EmptyScopedLock
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
