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
#include <corelib/ObjectWrapperCollection.h>

namespace core {

	class IStreamManagerReaderOperations
	{
	public:
		//! Typ wska�nika na strumie�
		typedef utils::shared_ptr<const std::istream> StreamConstPtr;

		//! Opis strumienia
		struct StreamData
		{
			StreamConstPtr stream;	//! Strumie�
			std::string path;		//! Protok�/�cie�ka strumienia
		};

		//! Lista danych strumieni
		typedef std::list<StreamData> StreamsList;

	public:

		virtual ~IStreamManagerReaderOperations() {}

		//! \param files Zbi�r plik�w ktrymi aktualnie zarz�dza ten DataManager
		virtual void getStreams(StreamsList & streams) const = 0;

		//! \param file Plik k�try weryfikujemy czy jest zarz�dzany przez DM
		//! \return Prawda je�li plik jest zarz�dzany przez ten DM
		virtual const bool isManaged(const std::istream * stream) const = 0;
		//! \param files Zbior plik�w dla kt�rych chcemy pobra� list� obiekt�w
		//! \return Mapa obiekt�w wzgl�dem plik�w z kt�rych pochodza
		virtual void getObjects(const std::istream * stream, ConstObjectsList & objects) const = 0;

		//! \param files Zbior plik�w dla kt�rych chcemy pobra� list� obiekt�w
		//! \return Mapa obiekt�w wzgl�dem plik�w z kt�rych pochodza
		virtual void getObjects(const std::istream * stream, ObjectWrapperCollection & objects) const = 0;
	};

	//! Interfejs zapewniaj�cy dost�p do danych strumieniowych zarzazanych przez aplikacj�
	class IStreamManagerReader : public IStreamManagerReaderOperations
	{
	public:
		//! Typ zmian danych w managerze
		enum ModificationType {
			ADD_STREAM,		//! Dodajemy strumie� do DM
			REMOVE_STREAM	//! Usuwamy strumie� z DM
		};

		//! Obiekt opisuj�cy zmian� w DM
		struct StreamChange
		{
			StreamConstPtr stream;			//! Strumie� kt�ry modyfikujemy
			std::string extension;			//! Rozszerzenie z jakim strumie� by� skojarzony
			ModificationType modyfication;	//! Typ zmiany na strumieniu
		};

		//! Agregat zmian w DM
		typedef std::list<StreamChange> ChangeList;

		//! Interfejs obserwatora zmian
		class IStreamObserver
		{
		public:
			//! \param changes Lista zmian managera
			virtual void observe(const ChangeList & changes) = 0;
		};

		//! Wska�nik na obiek obserwuj�cy zmiany
		typedef utils::shared_ptr<IStreamObserver> StreamObserverPtr;

		typedef utils::shared_ptr<IStreamManagerReaderOperations> TransactionPtr;

	public:
		//! \param streamWatcher Rejestrowany obiekt obserwuj�cy zmiany managera strumieni
		virtual void addObserver(const StreamObserverPtr & streamWatcher) = 0;
		//! \param streamWatcher Wyrejestrowywany obiekt obserwuj�cy zmiany managera strumieni
		virtual void removeObserver(const StreamObserverPtr & streamWatcher) = 0;

		virtual TransactionPtr transaction() const = 0;
	};

}

#endif	//	HEADER_GUARD___ISTREAMMANAGERREADER_H__
