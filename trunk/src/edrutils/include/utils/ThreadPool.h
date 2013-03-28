/********************************************************************
    created:  2013/03/26
    created:  26:3:2013   15:30
    filename: ThreadPool.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___THREADPOOL_H__
#define HEADER_GUARD___THREADPOOL_H__

#include <utils/IThreadPool.h>

namespace utils {

	class ThreadPool : public IThreadPool
	{
	private:
		//! Typ implementuj¹cy faktyczn¹ funkcjonalnoœæ
		class ThreadPoolImpl;

	private:
		//! Obiekt realizujacy funkcjonalnoœæ
		std::auto_ptr<ThreadPoolImpl> impl_;

	public:
		//! \param minThreads Minimalna iloœæ w¹tków do utrzymania
		//! \param maxThreads Maksymalna iloœæ w¹tków do utrzymania
		ThreadPool(size_type minThreads = 4, size_type maxThreads = 16);

		//! \return Maksymalna iloœc w¹tków jakie mo¿na utworzyæ
		virtual const size_type maxThreads() const;
		//! \return Minimalna iloœæ w¹tków utrzymywana przez manager
		virtual const size_type minThreads() const;
		//! \return Iloœæ aktualnie zajêtych w¹tków
		virtual const size_type threadsCount() const;
		//! \return Nowy w¹tek
		virtual ThreadPtr getOrCreateThread();
		//! \param groupSize Iloœæ w¹tków w grupie
		//! \param threads [out] Lista z nowymi w¹tkami, dopisujemy zawsze na koñcu
		virtual void getOrCreateThreadsGroup(const size_type groupSize, std::vector<ThreadPtr> & threads);
		//! \param maxThreads Maksymalna iloœæ w¹tków jakie mo¿na stworzyæ
		void setMaxThreads(size_type maxThreads);
		//! \param minThreads Minimalna iloœæ w¹tków jaka bêdzie utrzymywana
		void setMinThreads(size_type minThreads);
	};

}

#endif	//	HEADER_GUARD___THREADPOOL_H__
