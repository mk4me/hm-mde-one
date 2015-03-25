/********************************************************************
	created:  2014/10/22	07:34:27
	filename: InterruptFlag.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_THREADINGUTILS__INTERRUPTFLAG_H__
#define __HEADER_GUARD_THREADINGUTILS__INTERRUPTFLAG_H__

#include <atomic>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <threadingUtils/Macros.h>
#include <threadingUtils/Export.h>

namespace threadingUtils
{
	//! Klasa pomocnicza przy realizacji funkcjonalno�ci bezpiecznie przerywalnych w�tk�w
	class THREADINGUTILS_EXPORT InterruptFlag
	{
	private:
		//! Flaga czy w�tek faktycznie by� przerwany
		std::atomic<bool> flag;
		//! Zmienna warunkowa pozwalaj�ca przerywa� oczekiwanie
		std::condition_variable * threadCond;
		//! Zmienna warunkowa pozwalaj�ca przerywa� oczekiwanie w�asnych struktur synchronizacyjnych
		std::condition_variable_any * threadCondAny;
		//! Obiekt synchronizuj�cy
		std::mutex setClearMutex;

		//! \tparam Lockable Typ obiektu synchronizuj�cego
		//! Klasa wpiera funkcjonalno�c przerywania oczekiwania na customowych obiektach synchronizuj�cych
		template<typename Lockable>
		struct CustomLock
		{
		private:
			//! Flaga synchronizuj�ca
			InterruptFlag * self;
			//! Obiekt synchronizuj�cy
			Lockable & lk;

		public:
			//! RAII
			//! \param self Flaga synchronizuj�ca
			//! \param cond Zmienna warunkowa
			//! \param lk Obiekt synchronizuj�cy
			CustomLock(InterruptFlag * self_,
				std::condition_variable_any & cond,
				Lockable & lk) :
				self(self_), lk(lk)
			{
				self->setClearMutex.lock();
				self->threadCondAny = &cond;
			}

			//! Zwalnia obiekt synchronizuj�cy
			void unlock()
			{
				lk.unlock();
				self->setClearMutex.unlock();
			}

			//! Blokuje obiekt synchronizuj�cy
			void lock()
			{
				std::lock(self->setClearMutex, lk);
			}

			//! Destruktor
			//! Przywraca stan zasob�w flagi
			~CustomLock()
			{
				self->threadCondAny = 0;
				self->setClearMutex.unlock();
			}
		};	

		//! Struktura pomocnicza przy zwalnianiu zmiennej warunkowej w przypadku wyj�tku przerwania
		struct ClearConditionVariableOnDestruct
		{
			//! Destruktor
			~ClearConditionVariableOnDestruct();
		};

	public:

		//! Konstruktor domy�lny
		InterruptFlag();
		//! Destruktor
		~InterruptFlag();

		//! Ustawia flag� przerwania
		void set();

		//! Resetuje flag� przerwania
		void reset();

		//! \return Czy flaga przerwania jest ustawiona
		const bool isSet() const;

		//! \return Czy flaga aktualnego w�tku jest ustawiona
		static const bool threadIsSet();

		//! Resetuje flage przerwania aktualnego w�tku
		static void threadReset();

		//! \return Wskaźnik flagi przerwania aktualnego w�tku
		static InterruptFlag *& threadInterruptFlag();

		//! \param cv Zmienna warunkowa
		void setConditionVariable(std::condition_variable & cv);

		//! Zeruje zmienn� warunkow�
		void clearConditionVariable();

		//! Czekanie przerywalne na w�asnym obiekcie synchronizuj�cym
		template<typename Lockable>
		void wait(std::condition_variable_any& cv, Lockable& lk)
		{
			CustomLock<Lockable> cl(this, cv, lk);
			interruption_point();
			cv.wait(cl);
			interruption_point();
		}

		//! Czekanie przerywalne z predykatem na w�asnym obiekcie synchornizuj�cym
		template<typename Lockable, typename Predicate>
			void wait(std::condition_variable_any& cv, Lockable& lk, Predicate pred)
		{
			bool res = false;
			CustomLock<Lockable> cl(this, cv, lk);
			interruption_point();
			while ((threadInterruptFlag()->isSet() == false) && ((res = pred()) == false))
			{
				if(cv.wait_for(cl, std::chrono::milliseconds(0)) == std::cv_status::timeout){
					std::this_thread::yield();
				}
			}

			if (res == false){
				interruption_point();
			}
		}

		//! Czekanie przerywalne z predykatem na w�asnym obiekcie synchornizuj�cym z customowym timeoutem
		template<typename Lockable, typename Predicate, class Rep = long long,
		class Period = std::chrono::milliseconds>
		void wait(std::condition_variable_any& cv, Lockable& lk, Predicate pred,
			std::chrono::duration<Rep, Period> & timeout)
		{
			bool res = false;
			CustomLock<Lockable> cl(this, cv, lk);
			interruption_point();
			while ((threadInterruptFlag()->isSet() == false) && ((res = pred()) == false))
			{
				cv.wait_for(cl, timeout);
			}

			if (res == false){
				interruption_point();
			}
		}
	};
}

#endif	// __HEADER_GUARD_THREADINGUTILS__INTERRUPTFLAG_H__
