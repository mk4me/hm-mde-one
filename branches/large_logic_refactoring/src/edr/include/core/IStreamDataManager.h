/********************************************************************
    created:  2013/01/09
    created:  9:1:2013   8:04
    filename: IStreamDataManager.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___ISTREAMDATAMANAGER_H__
#define HEADER_GUARD___ISTREAMDATAMANAGER_H__

#include <core/SmartPtr.h>
#include <core/ITransaction.h>
#include <core/IStreamManagerReader.h>
#include <core/IMemoryDataManager.h>

namespace core {

	//! Zbi�r operacji na strumieniach obs�ugiwanych przez aplikacj�
	class IStreamDataManagerOperations
	{
	public:
		//! \data Dane wchodz�ce pod kontrol� DM
		virtual void addStream(std::istream * stream) = 0;
		//! Dane usuwane z DM
		virtual void removeStream(const std::istream * stream) = 0;

		virtual const bool tryAddData(std::istream * stream) = 0;

		virtual const bool tryRemoveStream(const std::istream * stream) = 0;
	};

	//! Manager udost�pniaj�cy operacje na strumieniach zarz�dzanych przez DM w oparciu o zarejestrowane parsery i rozszerzenia
	class IStreamDataManager : public IStreamDataManagerOperations
	{
	public:

		class IStreamDataManagerTransaction : public ITransaction, public IStreamDataManagerOperations, public IStreamManagerReaderOperations
		{

		};

		//! Typ transkacji na strumieniach - dzia�a w oparciu o RAII -> pr�buje "commitowa�" zmiany przy niszczeniu obiektu transakcji
		typedef shared_ptr<IStreamDataManagerTransaction> TransactionPtr;

	public:
		//! \return Transakcja na strumieniach
		virtual const TransactionPtr transaction() = 0;
	};

}

#endif	//	HEADER_GUARD___ISTREAMDATAMANAGER_H__
