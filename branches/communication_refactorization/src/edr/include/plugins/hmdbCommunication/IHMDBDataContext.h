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

	//! Klasa opisuj�ca podstawowy kontekst danych - storage + status danych + minimalny opis usera
	class IHMDBDataContext
	{
	public:
		//! Desturktor wirtualny
		virtual ~IHMDBDataContext() {}
		//! \return Obiekt zarz�dzaj�cy �ci�gni�tymi danymi
		virtual const IHMDBStorage * storage() const = 0;
		//! \return Obiekt zarz�dzaj�cy �ci�gni�tymi danymi
		virtual IHMDBStorage * storage() = 0;
		//! \return User hash na potrzeby obs�ugi p�ytkiej kopii bazy danych w storage
		virtual const std::string userName() const = 0;
		//! \return User hash na potrzeby obs�ugi p�ytkiej kopii bazy danych w storage
		virtual const std::string userHash() const = 0;
	};
}

#endif	// __HEADER_GUARD_HMDBCOMMUNICATION__IHMDBDATACONTEXT_H__