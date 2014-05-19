/********************************************************************
	created:  2013/07/04
	created:  4:7:2013   17:07
	filename: IJobManager.h
	author:   Mateusz Janiak

	purpose:  Interfejs dla wbudowanego managera jobów
	*********************************************************************/
#ifndef HEADER_GUARD_CORE__IJOBMANAGER_H__
#define HEADER_GUARD_CORE__IJOBMANAGER_H__

#include <corelib/IJob.h>

namespace core
{
	//! Interfejs managera zadañ
	class IJobManager
	{
	public:
		//! Wirualny destruktor
		virtual ~IJobManager() {}

		//! Metoda dodaje zadanie wraz z opisem zlecaj¹cego i opisem zadania do
		//! kolejki przetwarzania zwracaj¹c uchwyt do zadania
		virtual IJobPtr addJob(const std::string & who, const std::string & name,
			threadingUtils::IRunnablePtr runnable) = 0;
	};
}

#endif	//	HEADER_GUARD_CORE__IJOBMANAGER_H__
