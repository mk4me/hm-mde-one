/********************************************************************
    created:  2013/07/04
    created:  4:7:2013   21:33
    filename: IJob.h
    author:   Mateusz Janiak
    
    purpose:  Interfejs reprezentuj¹ca opakowane zadanie u¿ytkownika
*********************************************************************/
#ifndef HEADER_GUARD_CORE__IJOB_H__
#define HEADER_GUARD_CORE__IJOB_H__

#include <threading/Runnable.h>
#include <string>

namespace core
{	

	class IJob
	{
	public:
		//! Typ wyliczeniowy opisuj¹cy stan zadania
		enum Status {
			JOB_PENDING,	//! Oczekuje na przetwarzanie
			JOB_WORKING,	//! W trakcie
			JOB_FINISHED,	//! Skoñczone
			//JOB_CANCELED,	//! Anulowane
			JOB_ERROR		//! B³¹d
		};

	public:

		virtual ~IJob() {}

		//! \return Zadanie do przetworzenia
		virtual utils::IRunnablePtr runnable() = 0;
		//! \return Zadanie do przetworzenia
		virtual utils::IRunnableConstPtr runnable() const = 0;
		//! \return Nazwa zleceniodawcy
		virtual const std::string & who() const = 0;
		//! \return Nazwa zadania
		virtual const std::string & name() const = 0;
		//! \param Status zadania
		virtual const Status status() const = 0;	
	};

	DEFINE_SMART_POINTERS(IJob)

}

#endif	//	HEADER_GUARD_CORE__IJOB_H__
