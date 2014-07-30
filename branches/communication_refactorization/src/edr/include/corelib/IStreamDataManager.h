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

	//! Zbi�r operacji na strumieniach obs�ugiwanych przez aplikacj�
	class IStreamDataManagerOperations
	{
	public:

		//! Typ wska�nika na strumie�
		typedef utils::shared_ptr<std::istream> StreamPtr;

		//! Interfejs obiektu dostarczaj�cego strumie�
		class IStreamGrabber
		{
		public:
			//! Destruktor wirtualny
			virtual ~IStreamGrabber() {}
			//! \return Strumie�
			virtual const StreamPtr stream() const = 0;
			//! \return Nazwa, identyfikator strumienia
			virtual const std::string name() const = 0;
		};

		//! Wska�nik obiektu dostarczaj�cego struemienia
		typedef utils::shared_ptr<IStreamGrabber> StreamGrabberPtr;

	public:
		//! Destrutkor wirtualny
		virtual ~IStreamDataManagerOperations() {}

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

	//! Manager udost�pniaj�cy operacje na strumieniach zarz�dzanych przez DM w oparciu o zarejestrowane parsery i rozszerzenia
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

		//! Typ transkacji na strumieniach - dzia�a w oparciu o RAII -> pr�buje "commitowa�" zmiany przy niszczeniu obiektu transakcji
		typedef utils::shared_ptr<IStreamDataManagerTransaction> TransactionPtr;

	public:
		//! Destruktor wirtualny
		virtual ~IStreamDataManager() {}

		//! \return Transakcja na strumieniach
		virtual const TransactionPtr transaction() = 0;
	};

}

#endif	//	HEADER_GUARD___ISTREAMDATAMANAGER_H__
