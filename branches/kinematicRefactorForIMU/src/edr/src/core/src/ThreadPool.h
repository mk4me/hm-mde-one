/********************************************************************
	created:  2013/07/05
	created:  5:7:2013   13:31
	filename: ThreadPool.h
	author:   Mateusz Janiak

	purpose:  Implementacja puli w�tk�w
	*********************************************************************/
#ifndef HEADER_GUARD_CORE__THREADPOOL_H__
#define HEADER_GUARD_CORE__THREADPOOL_H__

#include <corelib/IThreadPool.h>
#include <threadingUtils/ThreadPool.h>
#include <corelib/BaseDataTypes.h>
#include <threadingUtils/SynchronizationPolicies.h>

namespace core
{
	class ThreadPool : public IThreadPool
	{
	public:

		//! \param threadFactory Fabryka w�tk�w
		//! \param minThreads Minimalna ilo�� w�tk�w jakie nale�y utrzymywa�
		//! \param maxThreads Maksymalna ilo�� w�tk�w
		ThreadPool(threadingUtils::IThreadFactoryPtr threadFactory,
			const size_type minThreads, const size_type maxThreads);

		//! Destruktor wirtualny
		virtual ~ThreadPool();

		//! \return Maksymalna ilo�� w�tk�w jak� oferuje pula
		virtual const size_type max() const;
		//! \return Minimalna ilo�� utrzymywanych w�tk�w jak� utrzymuje pula
		virtual const size_type min() const;
		//! \return Ilo�� wolnych aktualnie w�tk�w utrzymywanych przez pul�
		virtual const size_type free() const;
		//! \return Aktualna ilo�� w�tk�w
		virtual const size_type count() const;

		//! Metoda pobiera w�tki z puli podaj�c kto i po co pobiera
		//! \param who Kto pobiera w�tki
		//! \param[out] threads Agregat ��dowany pobieranymi w�tkami
		//! \param count Ilo�� w�tk�w do pobrania
		//! \param exact Czy mo�na pobra� mniej w�tk�w ni� ��dano
		virtual void getThreads(const std::string & who,
			Threads & threads, const size_type count, const bool exact);

		//! \param maxThreads Maksymalna ilo�� w�tk�w jakie mo�e dostarczy� pula
		void setMaxThreads(size_type maxThreads);
		//! \param minThreads Minimalna ilo�c w�tk�w jakie powinn utrzymywa� pula
		void setMinThreads(size_type minThreads);

	private:
		//! Faktyczny pool
		utils::shared_ptr<threadingUtils::ThreadPool> threadPool_;
		//! Obiekt synchronizuj�cy
		mutable threadingUtils::StrictSyncPolicy synch_;
	};
}

#endif	//	HEADER_GUARD_CORE__THREADPOOL_H__
