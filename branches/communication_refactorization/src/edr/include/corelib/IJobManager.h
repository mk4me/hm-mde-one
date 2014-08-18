/********************************************************************
	created:  2013/07/04
	created:  4:7:2013   17:07
	filename: IJobManager.h
	author:   Mateusz Janiak

	purpose:  Interfejs dla wbudowanego managera job�w
	*********************************************************************/
#ifndef HEADER_GUARD_CORE__IJOBMANAGER_H__
#define HEADER_GUARD_CORE__IJOBMANAGER_H__

#include <corelib/IJob.h>

namespace core
{
	//! Interfejs managera zada�
	class IJobManager
	{
	public:
		//! Wirualny destruktor
		virtual ~IJobManager() {}

		//! Metoda dodaje zadanie wraz z opisem zlecaj�cego i opisem zadania do
		//! kolejki przetwarzania zwracaj�c uchwyt do zadania
		//! \param who Kto dodaje zadanie
		//! \param name Nazwa zadania
		//! \return Zadanie
		virtual IJobPtr addJob(const std::string & who, const std::string & name,
			threadingUtils::IRunnablePtr runnable) = 0;
		//! \param who Kto dodaje zadanie
		//! \param name Nazwa zadania
		//! \param f Funkcja do wykonania
		//! \return Powrot kiedy zakonczymy przetwarzanie f, prawda jesli zakonczylo sie sukcesem
		virtual const bool execute(const std::string & who, const std::string & name,
			threadingUtils::FunctorRunnable::Functor f) = 0;
	};
}

#endif	//	HEADER_GUARD_CORE__IJOBMANAGER_H__
