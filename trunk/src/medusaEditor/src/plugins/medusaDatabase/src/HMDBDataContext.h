/********************************************************************
	created:  2014/07/11	17:36:28
	filename: HMDBDataContext.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_medusaDatabase__HMDBDATACONTEXT_H__
#define __HEADER_GUARD_medusaDatabase__HMDBDATACONTEXT_H__

#include <plugins/medusaDatabase/IHMDBDataContext.h>

namespace medusaDatabase
{
	class IHMDBStorage;

	class HMDBDataContext : public IHMDBDataContext
	{
	public:
		//! \param storage Storage do zapisu plik�w z danymi i p�ytkiej kopii bazy danych
		//! \param userName Skr�t u�ytkownika na potrzby obs�ugi p�ytkiej kopii
		//! \param userHash Nazwa u�ytkownika
		HMDBDataContext(IHMDBStoragePtr storage, const std::string & userName,
			const std::string & userHash);
		//! Destruktor wirtualny
		virtual ~HMDBDataContext();
		//! \return Obiekt zarz�dzaj�cy �ci�gni�tymi danymi
		virtual const IHMDBStorageConstPtr storage() const;
		//! \return Obiekt zarz�dzaj�cy �ci�gni�tymi danymi
		virtual const IHMDBStoragePtr storage();
		//! \return User hash na potrzeby obs�ugi p�ytkiej kopii bazy danych w storage
		virtual const std::string userName() const;
		//! \return User hash na potrzeby obs�ugi p�ytkiej kopii bazy danych w storage
		virtual const std::string userHash() const;
		//! \return P�ytka kopia bazy danych
		virtual const ShallowCopyPtr createShallowCopy() const;
		//! \return Czy p�ytka kopia wyst�puje
		virtual const bool shallowCopyExists() const;

	private:
		//! Storage do zapisu plik�w z danymi i p�ytkiej kopii bazy danych
		IHMDBStoragePtr storage_;
		//! Skr�t u�ytkownika na potrzby obs�ugi p�ytkiej kopii
		const std::string userHash_;
		//! Nazwa u�ytkownika
		const std::string userName_;
	};
}

#endif	// __HEADER_GUARD_medusaDatabase__HMDBDATACONTEXT_H__
