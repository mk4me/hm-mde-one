/********************************************************************
	created:  2014/05/21	13:24:59
	filename: IHMDBSourceContext.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_HMDBCOMMUNICATION__IHMDBSOURCECONTEXT_H__
#define __HEADER_GUARD_HMDBCOMMUNICATION__IHMDBSOURCECONTEXT_H__

#include <plugins/hmdbCommunication/DataStatus.h>
#include <plugins/hmdbCommunication/ShallowCopy.h>
#include <plugins/hmdbCommunication/IHMDBFtp.h>
#include <hmdbserviceslib/DateTime.h>
#include <corelib/Variant.h>
#include <threadingUtils/IOperation.h>
#include <threadingUtils/IProgress.h>

namespace hmdbCommunication
{
	//! Forward declarations
	class IHMDBLocalContext;	
	class IHMDBRemoteContext;
	class IHMDBDataContext;

	//! Interfejs opisuj¹cy kontekst Ÿród³a danych
	class IHMDBSourceContext
	{
	public:
		//! Destruktor wirtualny
		virtual ~IHMDBSourceContext() {}
		
		//! Kontekst lokalny
		virtual IHMDBLocalContext * localContext() = 0;
		virtual const IHMDBLocalContext * localContext() const = 0;

		//! Kontekst zdalny
		virtual IHMDBRemoteContext * remoteContext() = 0;
		virtual const IHMDBRemoteContext * remoteContext() const = 0;

		//! Kontekst danych
		virtual IHMDBDataContext * dataContext() = 0;
		virtual const IHMDBDataContext * dataContext() const = 0;
	};
}

#endif	// __HEADER_GUARD_HMDBCOMMUNICATION__IHMDBSOURCECONTEXT_H__