/********************************************************************
	created:  2014/10/28	13:12:42
	filename: IJob.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_THREADINGUTILS__IJOB_H__
#define __HEADER_GUARD_THREADINGUTILS__IJOB_H__

namespace threadingUtils
{
	//! Interfejs klasy realizuj�cej zadanie
	class IJob
	{
	public:
		//! Typ wyliczeniowy opisuj�cy stan zadania
		enum Status {
			Initialized,	//! Zadanie zainicjalizowane
			Pending,		//! Zadanie oczekuj�ce na wykonanie (wystartowano, mo�na czeka� na zadaniu)
			Running,		//! Zadanie w trakcie wykonania
			Finished,		//! Zadanie zako�czone poprawnie
			Cancelled,		//! Zadanie przerwane (anulowano)
			Failed			//! Zadanie zako�czone niepowodzeniem
		};

	public:
		//! Destruktor wirtualny
		virtual ~IJob() {}
		//! Metoda startuj�ca zadanie, mo�e rzuca� wyj�tkami
		virtual void start() = 0;
		//! Metoda czekaj�ca na zako�czenie zadania
		virtual void wait() = 0;
		//! Metoda pozwalaj�ca zadaniu wykonywa� si� dalej w tle
		virtual void detach() = 0;
		//! Metoda anuluj�ca zadanie
		virtual void cancel() = 0;
		//! \return Czy zadanie mo�na jeszcze anulowa�
		virtual const bool cancellable() const = 0;
		//! \return Aktualny status zadania
		virtual const Status status() const = 0;
	};
}

#endif	// __HEADER_GUARD_THREADINGUTILS__IJOB_H__