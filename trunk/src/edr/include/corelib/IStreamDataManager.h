/********************************************************************
    created:  2013/01/09
    created:  9:1:2013   8:04
    filename: IStreamDataManager.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___ISTREAMDATAMANAGER_H__
#define HEADER_GUARD___ISTREAMDATAMANAGER_H__

#include <utils/SmartPtr.h>
#include <corelib/ITransaction.h>
#include <corelib/IStreamManagerReader.h>
#include <corelib/IMemoryDataManager.h>

namespace core {

	//! Zbiór operacji na strumieniach obsługiwanych przez aplikację
	class IStreamDataManagerOperations
	{
	public:

		virtual ~IStreamDataManagerOperations() {}

		//! \data Dane wchodzące pod kontrolę DM
		virtual void addStream(std::istream * stream) = 0;
		//! Dane usuwane z DM
		virtual void removeStream(const std::istream * stream) = 0;

		virtual const bool tryAddData(std::istream * stream) = 0;

		virtual const bool tryRemoveStream(const std::istream * stream) = 0;
	};

	//! Manager udostępniający operacje na strumieniach zarządzanych przez DM w oparciu o zarejestrowane parsery i rozszerzenia
	class IStreamDataManager : public IStreamDataManagerOperations
	{
	public:

		class IStreamDataManagerTransaction : public ITransaction, public IStreamDataManagerOperations, public IStreamManagerReaderOperations
		{
		public:
			virtual ~IStreamDataManagerTransaction() {}
		};

		//! Typ transkacji na strumieniach - działa w oparciu o RAII -> próbuje "commitować" zmiany przy niszczeniu obiektu transakcji
		typedef utils::shared_ptr<IStreamDataManagerTransaction> TransactionPtr;

	public:

		virtual ~IStreamDataManager() {}

		//! \return Transakcja na strumieniach
		virtual const TransactionPtr transaction() = 0;
	};

}

#endif	//	HEADER_GUARD___ISTREAMDATAMANAGER_H__
