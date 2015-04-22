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

	public:

		//! Struktura pomocnicza przy zwalnianiu zmiennej warunkowej w przypadku wyj�tku przerwania
		struct ClearConditionVariableOnDestruct
		{
			//! Destruktor
			~ClearConditionVariableOnDestruct()
			{
				threadInterruptFlag()->clearConditionVariable();
			}
		};

		//! \tparam Lockable Typ obiektu synchronizuj�cego
		//! Klasa wpiera funkcjonalno�c przerywania oczekiwania na customowych obiektach synchronizuj�cych
		template<typename Lockable>
		class CustomLock
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
				self->threadCondAny = nullptr;
				self->setClearMutex.unlock();
			}
		};

		template<typename Lockable>
		friend class CustomLock;

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
	};
}

#endif	// __HEADER_GUARD_THREADINGUTILS__INTERRUPTFLAG_H__
