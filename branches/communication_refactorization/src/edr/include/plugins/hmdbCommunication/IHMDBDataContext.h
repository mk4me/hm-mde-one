/********************************************************************
	created:  2014/07/04	10:30:09
	filename: IHMDBDataContext.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_HMDBCOMMUNICATION__IHMDBDATACONTEXT_H__
#define __HEADER_GUARD_HMDBCOMMUNICATION__IHMDBDATACONTEXT_H__

#include <string>

namespace hmdbCommunication
{
	//! Forward declarations
	class IHMDBStorage;

	//! Klasa opisująca podstawowy kontekst danych - storage + status danych + minimalny opis usera
	class IHMDBDataContext
	{
	public:
		//! Desturktor wirtualny
		virtual ~IHMDBDataContext() {}
		//! \return Obiekt zarządzający ściągniętymi danymi
		virtual const IHMDBStorage * storage() const = 0;
		//! \return Obiekt zarządzający ściągniętymi danymi
		virtual IHMDBStorage * storage() = 0;
		//! \return User hash na potrzeby obsługi płytkiej kopii bazy danych w storage
		virtual const std::string userName() const = 0;
		//! \return User hash na potrzeby obsługi płytkiej kopii bazy danych w storage
		virtual const std::string userHash() const = 0;
	};
}

#endif	// __HEADER_GUARD_HMDBCOMMUNICATION__IHMDBDATACONTEXT_H__