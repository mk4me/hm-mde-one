/********************************************************************
    created:  2013/01/08
    created:  8:1:2013   20:08
    filename: IMemoryDataManager.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___IMEMORYDATAMANAGER_H__
#define HEADER_GUARD___IMEMORYDATAMANAGER_H__

#include <core/ITransaction.h>
#include <core/IDataManagerReader.h>
#include <core/ObjectWrapper.h>
#include <utils/SynchronizationPolicies.h>

namespace core {

	//! Operacje zwi�zane z zarz�dzaniem danymi domenowymi aplikacji
	class IMemoryDataManagerOperations
	{
	public:
		//! \data Dane wchodz�ce pod kontrol� DM
		virtual void addData(const ObjectWrapperPtr & data) = 0;
		//! Dane usuwane z DM
		virtual void removeData(const ObjectWrapperConstPtr & data) = 0;
		//! \param data Aktualizowane dane
		//! \param newData Nowa warto�� danych
		virtual void updateData(const ObjectWrapperConstPtr & data, const ObjectWrapperConstPtr & newData) = 0;

		virtual const bool tryAddData(const ObjectWrapperPtr & data) = 0;

		virtual const bool tryRemoveData(const ObjectWrapperConstPtr & data) = 0;

		virtual const bool tryUpdateData(const ObjectWrapperConstPtr & data, const ObjectWrapperConstPtr & newData) = 0;
	};

	//! Interfejs do danych domenowych i zarz�dzania nimi w aplikacji
	class IMemoryDataManager : public IMemoryDataManagerOperations
	{
	public:

		class IMemoryDataTransaction : public ITransaction, public IMemoryDataManagerOperations, public IDataManagerReaderOperations
		{

		};

		//! Typ transakcji na danych domenowych - dzia�a w oparciu o RAII -> pr�buje "commitowa�" zmiany przy ko�cu trwania �ycia obiektu transakcji
		typedef core::shared_ptr<IMemoryDataTransaction> TransactionPtr;

	public:
		//! \return Nowa transakcja danych domenowych
		virtual TransactionPtr transaction() = 0;
	};

}

#endif	//	HEADER_GUARD___IMEMORYDATAMANAGER_H__
