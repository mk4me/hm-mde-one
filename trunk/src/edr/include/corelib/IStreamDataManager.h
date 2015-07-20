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
#include <corelib/IDataManager.h>
#include <corelib/IParser.h>

namespace core {

	//! Manager udost�pniaj�cy operacje na strumieniach zarz�dzanych przez DM w oparciu o zarejestrowane parsery i rozszerzenia
	class IStreamDataManager : public IStreamManagerReader
	{
	public:

		//! Typ strumienia dla grabbera
		typedef plugin::IStreamParser::IStreamPtr IStreamPtr;

		//! Interfejs obiektu dostarczaj�cego strumie�
		class IStreamGrabber
		{
		public:
			//! Destruktor wirtualny
			virtual ~IStreamGrabber() {}
			//! \return Strumie�
			virtual const IStreamPtr stream() const = 0;
			//! \return Nazwa, identyfikator strumienia
			virtual const std::string name() const = 0;
		};

		//! Wska�nik obiektu dostarczaj�cego struemienia
		typedef utils::shared_ptr<IStreamGrabber> StreamGrabberPtr;

		//! Zbi�r operacji na strumieniach obs�ugiwanych przez aplikacj�
		class IOperations
		{
		public:
			//! Destrutkor wirtualny
			virtual ~IOperations() {}

			//! \param stream Strumie� dodawany do managera
			//! Rzuca wyj�tkiem je�li co� si� nie powiedzie
			virtual void addStream(const StreamGrabberPtr streamGrabber) = 0;
			//! \param stream Strumie� usuwany z DM
			//! Rzuca wyj�tkiem je�li co� si� nie powiedzie
			virtual void removeStream(const std::string & stream) = 0;
			//! \param stream Strumie� dodawany do managera
			//! \return Prawda je�li pomy�lnie dodano strumie�
			virtual const bool tryAddStream(const StreamGrabberPtr streamGrabber) = 0;
			//! \param stream Strumie� usuwany z managera
			//! \return Prawda je�li pomy�lnie usuni�to strumie�
			virtual const bool tryRemoveStream(const std::string & stream) = 0;
		};

		//! Interfejs transakcji managera strumieni
		class ITransaction : public core::ITransaction, public IOperations, public IStreamManagerReader::IOperations
		{
		public:
			//! Destruktor wirtualny
			virtual ~ITransaction() {}
		};

		//! Typ transkacji na strumieniach - dzia�a w oparciu o RAII -> pr�buje "commitowa�" zmiany przy niszczeniu obiektu transakcji
		typedef utils::shared_ptr<ITransaction> TransactionPtr;

	public:
		//! Destruktor wirtualny
		virtual ~IStreamDataManager() {}

		//! \return Transakcja na strumieniach
		virtual const TransactionPtr transaction() = 0;
	};

}

#endif	//	HEADER_GUARD___ISTREAMDATAMANAGER_H__
