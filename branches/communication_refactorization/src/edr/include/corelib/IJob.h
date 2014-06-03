/********************************************************************
	created:  2013/07/04
	created:  4:7:2013   21:33
	filename: IJob.h
	author:   Mateusz Janiak

	purpose:  Interfejs reprezentuj�ca opakowane zadanie u�ytkownika
	*********************************************************************/
#ifndef HEADER_GUARD_CORE__IJOB_H__
#define HEADER_GUARD_CORE__IJOB_H__

#include <threadingUtils/Runnable.h>
#include <string>

namespace core
{
	//! Interfejs klasy reprezentuj�cej zadanie do wykonania
	class IJob
	{
	public:
		//! Typ wyliczeniowy opisuj�cy stan zadania
		enum Status {
			JOB_PENDING,	//! Oczekuje na przetwarzanie
			JOB_WORKING,	//! W trakcie
			JOB_FINISHED,	//! Sko�czone
			//JOB_CANCELED,	//! Anulowane
			JOB_ERROR		//! B��d
		};

	public:
		//Desturuktor wirtualny
		virtual ~IJob() {}
		//! \return Zadanie do przetworzenia
		virtual threadingUtils::IRunnablePtr runnable() = 0;
		//! \return Zadanie do przetworzenia
		virtual threadingUtils::IRunnableConstPtr runnable() const = 0;
		//! \return Nazwa zleceniodawcy
		virtual const std::string & who() const = 0;
		//! \return Nazwa zadania
		virtual const std::string & name() const = 0;
		//! \param Status zadania
		virtual const Status status() const = 0;
		//! Metoda czeka a� zadanie nie dobiegnie ko�ca lub zostanie anulowane
		virtual void wait() = 0;
	};

	DEFINE_SMART_POINTERS(IJob)
}

#endif	//	HEADER_GUARD_CORE__IJOB_H__