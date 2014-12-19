/********************************************************************
	created:  2013/07/05
	created:  5:7:2013   13:31
	filename: ThreadPool.h
	author:   Mateusz Janiak

	purpose:  Implementacja puli w¹tków
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

		//! \param threadFactory Fabryka w¹tków
		//! \param minThreads Minimalna iloœæ w¹tków jakie nale¿y utrzymywaæ
		//! \param maxThreads Maksymalna iloœæ w¹tków
		ThreadPool(threadingUtils::IThreadFactoryPtr threadFactory,
			const size_type minThreads, const size_type maxThreads);

		//! Destruktor wirtualny
		virtual ~ThreadPool();

		//! \return Maksymalna iloœæ w¹tków jak¹ oferuje pula
		virtual const size_type max() const;
		//! \return Minimalna iloœæ utrzymywanych w¹tków jak¹ utrzymuje pula
		virtual const size_type min() const;
		//! \return Iloœæ wolnych aktualnie w¹tków utrzymywanych przez pulê
		virtual const size_type free() const;
		//! \return Aktualna iloœæ w¹tków
		virtual const size_type count() const;

		//! Metoda pobiera w¹tki z puli podaj¹c kto i po co pobiera
		//! \param who Kto pobiera w¹tki
		//! \param[out] threads Agregat ³¹dowany pobieranymi w¹tkami
		//! \param count Iloœæ w¹tków do pobrania
		//! \param exact Czy mo¿na pobraæ mniej w¹tków ni¿ ¿¹dano
		virtual void getThreads(const std::string & who,
			Threads & threads, const size_type count, const bool exact);

		//! \param maxThreads Maksymalna iloœæ w¹tków jakie mo¿e dostarczyæ pula
		void setMaxThreads(size_type maxThreads);
		//! \param minThreads Minimalna iloœc w¹tków jakie powinn utrzymywaæ pula
		void setMinThreads(size_type minThreads);

	private:
		//! Faktyczny pool
		utils::shared_ptr<threadingUtils::ThreadPool> threadPool_;
		//! Obiekt synchronizuj¹cy
		mutable threadingUtils::StrictSyncPolicy synch_;
	};
}

#endif	//	HEADER_GUARD_CORE__THREADPOOL_H__
