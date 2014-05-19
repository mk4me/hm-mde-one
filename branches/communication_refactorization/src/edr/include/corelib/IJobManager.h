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
		virtual IJobPtr addJob(const std::string & who, const std::string & name,
			threadingUtils::IRunnablePtr runnable) = 0;
	};
}

#endif	//	HEADER_GUARD_CORE__IJOBMANAGER_H__
