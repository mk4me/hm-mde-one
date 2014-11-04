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
	//! Klasa pomocnicza przy realizacji funkcjonalnoœci bezpiecznie przerywalnych w¹tków
	class THREADINGUTILS_EXPORT InterruptFlag
	{
	private:
		//! Flaga czy w¹tek faktycznie by³ przerwany
		std::atomic<bool> flag;
		//! Zmienna warunkowa pozwalaj¹ca przerywaæ oczekiwanie
		std::condition_variable * threadCond;
		//! Zmienna warunkowa pozwalaj¹ca przerywaæ oczekiwanie w³asnych struktur synchronizacyjnych
		std::condition_variable_any * threadCondAny;
		//! Obiekt synchronizuj¹cy
		std::mutex setClearMutex;

		//! \tparam Lockable Typ obiektu synchronizuj¹cego
		//! Klasa wpiera funkcjonalnoœc przerywania oczekiwania na customowych obiektach synchronizuj¹cych
		template<typename Lockable>
		struct CustomLock
		{
		private:
			//! Flaga synchronizuj¹ca
			InterruptFlag * self;
			//! Obiekt synchronizuj¹cy
			Lockable & lk;

		public:
			//! RAII
			//! \param self Flaga synchronizuj¹ca
			//! \param cond Zmienna warunkowa
			//! \param lk Obiekt synchronizuj¹cy
			CustomLock(InterruptFlag * self_,
				std::condition_variable_any & cond,
				Lockable & lk) :
				self(self_), lk(lk)
			{
				self->setClearMutex.lock();
				self->threadCondAny = &cond;
			}

			//! Zwalnia obiekt synchronizuj¹cy
			void unlock()
			{
				lk.unlock();
				self->setClearMutex.unlock();
			}

			//! Blokuje obiekt synchronizuj¹cy
			void lock()
			{
				std::lock(self->setClearMutex, lk);
			}

			//! Destruktor
			//! Przywraca stan zasobów flagi
			~CustomLock()
			{
				self->threadCondAny = 0;
				self->setClearMutex.unlock();
			}
		};	

		//! Struktura pomocnicza przy zwalnianiu zmiennej warunkowej w przypadku wyj¹tku przerwania
		struct ClearConditionVariableOnDestruct
		{
			//! Destruktor
			~ClearConditionVariableOnDestruct();
		};

	public:

		//! Konstruktor domyœlny
		InterruptFlag();
		//! Destruktor
		~InterruptFlag();

		//! Ustawia flagê przerwania
		void set();

		//! Resetuje flagê przerwania
		void reset();

		//! \return Czy flaga przerwania jest ustawiona
		const bool isSet() const;

		//! \return Czy flaga aktualnego w¹tku jest ustawiona
		static const bool threadIsSet();

		//! Resetuje flage przerwania aktualnego w¹tku
		static void threadReset();

		//! \return WskaŸnik flagi przerwania aktualnego w¹tku
		static InterruptFlag *& threadInterruptFlag();

		//! \param cv Zmienna warunkowa
		void setConditionVariable(std::condition_variable & cv);

		//! Zeruje zmienn¹ warunkow¹
		void clearConditionVariable();

		//! Czekanie przerywalne na w³asnym obiekcie synchronizuj¹cym
		template<typename Lockable>
		void wait(std::condition_variable_any& cv, Lockable& lk)
		{
			CustomLock cl(this, cv, lk);
			interruption_point();
			cv.wait(cl);
			interruption_point();
		}

		//! Czekanie przerywalne z predykatem na w³asnym obiekcie synchornizuj¹cym
		template<typename Lockable, typename Predicate>
			void wait(std::condition_variable_any& cv, Lockable& lk, Predicate pred)
		{
			bool res = false;
			CustomLock cl(this, cv, lk);
			interruption_point();
			while ((threadInterruptFlag()->isSet() == false) && ((res = pred()) == false))
			{
				if(cv.wait_for(cl, std::chrono::milliseconds(0)) == std::timeout){
					std::this_thread::yield();
				}
			}

			if (res == false){
				interruption_point();
			}
		}

		template<typename Lockable, typename Predicate, class Rep = long long,
		class Period = std::mili>
		void wait(std::condition_variable_any& cv, Lockable& lk, Predicate pred,
			std::chrono::duration<Rep, Period> & timeout)
		{
			bool res = false;
			CustomLock cl(this, cv, lk);
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