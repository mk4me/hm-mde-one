/********************************************************************
	created:  2014/07/04	10:30:09
	filename: IHMDBDataContext.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_medusaDatabase__IHMDBDATACONTEXT_H__
#define __HEADER_GUARD_medusaDatabase__IHMDBDATACONTEXT_H__

#include <string>
#include <plugins/medusaDatabase/ShallowCopy.h>
#include <plugins/medusaDatabase/IHMDBStorage.h>

namespace medusaDatabase
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
		virtual const IHMDBStorageConstPtr storage() const = 0;
		//! \return Obiekt zarz�dzaj�cy �ci�gni�tymi danymi
		virtual const IHMDBStoragePtr storage() = 0;
		//! \return User hash na potrzeby obs�ugi p�ytkiej kopii bazy danych w storage
		virtual const std::string userName() const = 0;
		//! \return User hash na potrzeby obs�ugi p�ytkiej kopii bazy danych w storage
		virtual const std::string userHash() const = 0;
		//! \return P�ytka kopia bazy danych
		virtual const ShallowCopyPtr createShallowCopy() const = 0;
		//! \return Czy p�ytka kopia wyst�puje
		virtual const bool shallowCopyExists() const = 0;
	};

	DEFINE_SMART_POINTERS(IHMDBDataContext);
}

#endif	// __HEADER_GUARD_medusaDatabase__IHMDBDATACONTEXT_H__
