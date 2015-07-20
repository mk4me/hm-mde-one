/********************************************************************
    created:  2013/01/08
    created:  8:1:2013   23:11
    filename: IStreamManagerReader.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___ISTREAMMANAGERREADER_H__
#define HEADER_GUARD___ISTREAMMANAGERREADER_H__

#include <iostream>
#include <corelib/BaseDataTypes.h>
#include <corelib/VariantsCollection.h>

namespace core {

	//! Interfejs zapewniaj�cy dost�p do danych strumieniowych zarzazanych przez aplikacj�
	class IStreamManagerReader
	{
	public:

		//! Lista danych strumieni
		typedef std::list<std::string> StreamsList;

		class IOperations
		{
		public:
			//! Destruktor wirtualny
			virtual ~IOperations() {}

			//! \param files Zbi�r plik�w ktrymi aktualnie zarz�dza ten DataManager
			virtual void getStreams(StreamsList & streams) const = 0;

			//! \param stream Nazwa strumienia
			//! \return Prawda je�li strumie� jest zarz�dzany przez ten DM
			virtual const bool isManaged(const std::string & stream) const = 0;

			//! \param stream Strumie� kt�ry weryfikujemy czy jest w pe�ni za�adowany
			//! \return Prawda je�li strumie� jest w pe�ni za�adowany
			virtual const bool isLoadedCompleately(const std::string & stream) const = 0;

			//! \param stream Nazwa strumienia
			//! \return Mapa obiekt�w wzgl�dem plik�w z kt�rych pochodza
			virtual void getObjects(const std::string & stream, ConstVariantsList & objects) const = 0;
		};

		//! Typ zmian danych w managerze
		enum ModificationType {
			ADD_STREAM,				//! Dodajemy strumie� do DM
			REMOVE_STREAM,			//! Usuwamy strumie� z DM
			REMOVE_UNUSED_STREAM	//! Usuwamy strumie� dla kt�rego nie ma ju� danych w Memory DM
		};

		//! Obiekt opisuj�cy zmian� w DM
		struct Change
		{
			std::string stream;				//! Strumie�
			ModificationType modyfication;	//! Typ zmiany na strumieniu
		};

		//! Agregat zmian w DM
		typedef std::list<Change> ChangeList;

		//! Interfejs obserwatora zmian
		typedef IChangesObserver<ChangeList> IObserver;

		//! Wska�nik na obiek obserwuj�cy zmiany
		typedef utils::shared_ptr<IObserver> ObserverPtr;
		//! Wska�nik transakcji
		typedef utils::shared_ptr<IOperations> TransactionPtr;

	public:

		//! \param protocol Nazwa protoko�y na kt�rym dost�pny jest strumie�
		//! \param source Nazwa strumienia z pomini�ciem protoko�u
		//! \return Pe�na nazwa strumienia
		inline static const std::string streamName(const std::string & protocol,
			const std::string & source)
		{
			return protocol + "://" + source;
		}

		//! \param streamWatcher Rejestrowany obiekt obserwuj�cy zmiany managera strumieni
		virtual void addObserver(const ObserverPtr & streamWatcher) = 0;
		//! \param streamWatcher Wyrejestrowywany obiekt obserwuj�cy zmiany managera strumieni
		virtual void removeObserver(const ObserverPtr & streamWatcher) = 0;
		//! \return Nowa transakcja
		virtual const TransactionPtr transaction() const = 0;
	};

}

#endif	//	HEADER_GUARD___ISTREAMMANAGERREADER_H__
