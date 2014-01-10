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

	//! Zbiór operacji na strumieniach obs³ugiwanych przez aplikacjê
	class IStreamDataManagerOperations
	{
	public:
		//! Destrutkor wirtualny
		virtual ~IStreamDataManagerOperations() {}

		//! \param stream Strumieñ dodawany do managera
		//! Rzuca wyj¹tkiem jeœli coœ siê nie powiedzie
		virtual void addStream(const IStreamManagerReaderOperations::StreamData & stream) = 0;
		//! \param stream Strumieñ usuwany z DM
		//! Rzuca wyj¹tkiem jeœli coœ siê nie powiedzie
		virtual void removeStream(const std::istream * stream) = 0;
		//! \param stream Strumieñ dodawany do managera
		//! \return Prawda jeœli pomyœlnie dodano strumieñ
		virtual const bool tryAddStream(const IStreamManagerReaderOperations::StreamData & stream) = 0;
		//! \param stream Strumieñ usuwany z managera
		//! \return Prawda jeœli pomyœlnie usuniêto strumieñ
		virtual const bool tryRemoveStream(const std::istream * stream) = 0;

		//! TODO
		// poprawiæ API - usuwamy smart pointerem strumienia albo jego œcie¿k¹
	};

	//! Manager udostêpniaj¹cy operacje na strumieniach zarz¹dzanych przez DM w oparciu o zarejestrowane parsery i rozszerzenia
	class IStreamDataManager : public IStreamDataManagerOperations
	{
	public:
		//! Interfejs transakcji managera strumieni
		class IStreamDataManagerTransaction : public ITransaction, public IStreamDataManagerOperations, public IStreamManagerReaderOperations
		{
		public:
			//! Destruktor wirtualny
			virtual ~IStreamDataManagerTransaction() {}
		};

		//! Typ transkacji na strumieniach - dzia³a w oparciu o RAII -> próbuje "commitowaæ" zmiany przy niszczeniu obiektu transakcji
		typedef utils::shared_ptr<IStreamDataManagerTransaction> TransactionPtr;

	public:
		//! Destruktor wirtualny
		virtual ~IStreamDataManager() {}

		//! \return Transakcja na strumieniach
		virtual const TransactionPtr transaction() = 0;
	};

}

#endif	//	HEADER_GUARD___ISTREAMDATAMANAGER_H__
