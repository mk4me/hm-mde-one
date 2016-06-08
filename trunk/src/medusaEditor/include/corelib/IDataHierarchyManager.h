/********************************************************************
	created:  2015/07/20	12:53:56
	filename: IDataHierarchyManager.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD___IDATAHIERARCHYMANAGER_H__
#define __HEADER_GUARD___IDATAHIERARCHYMANAGER_H__

#include <corelib/ITransaction.h>
#include <corelib/IDataHierarchyManagerReader.h>

namespace core
{
	class IDataHierarchyManager : public IDataHierarchyManagerReader
	{
	public:

		//! Operacje zwi�zane z zarz�dzaniem hierarchicznymi danymi
		class IOperations : public IDataHierarchyManagerReader::IOperations
		{
		public:
			//! Destruktor wirtualny
			virtual ~IOperations() {}

			//! Dodanie roota, odczytanie wszystkich danych w hierarchii i dodanie ich do DM
			//! \param ptr dodawany element
			virtual void addRoot(IHierarchyItemConstPtr ptr) = 0;
			//! Usuni�cie roota z DM
			//! \param ptr element do usuni�cia, musi by� wcze�niej dodany
			virtual void removeRoot(IHierarchyItemConstPtr ptr) = 0;
			//! Wywo�ane, gdy hierarchia, lub jej dane si� zmieni�y
			//! \param ptr update'owany element
			virtual void updateRoot(IHierarchyItemConstPtr ptr) = 0;
		};

		class ITransaction : public core::ITransaction, public IOperations
		{
		public:
			virtual ~ITransaction() {}
		};

		//! Typ transakcji dodaj�cej dane w postaci hierarchicznej
		typedef utils::shared_ptr<ITransaction> TransactionPtr;

	public:
		virtual ~IDataHierarchyManager() {}

		virtual TransactionPtr transaction() = 0;
	};
}

#endif	// __HEADER_GUARD___IDATAHIERARCHYMANAGER_H__