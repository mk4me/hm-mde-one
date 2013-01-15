/********************************************************************
    created:  2013/01/08
    created:  8:1:2013   23:11
    filename: IStreamManagerReader.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___ISTREAMMANAGERREADER_H__
#define HEADER_GUARD___ISTREAMMANAGERREADER_H__

#include <core/ObjectWrapper.h>
#include <core/ObjectWrapperCollection.h>

namespace std
{
	class ifstream;
}

namespace plugin {

	//! Interfejs zapewniaj¹cy dostêp do danych strumieniowych zarzazanych przez aplikacjê
	class IStreamManagerReader
	{
	public:
		//! Typ wskaŸnika na strumieñ
		typedef core::shared_ptr<const std::istream> StreamConstPtr;
		//! Opis strumienia
		struct StreamData
		{
			StreamConstPtr stream;	//! Strumieñ
			std::string path;		//! Protokó³/œcie¿ka strumienia
		};

		//! Lista danych strumieni
		typedef std::list<StreamData> StreamsList;

		//! Typ zmian danych w managerze
		enum ModificationType {
			ADD_STREAM,		//! Dodajemy strumieñ do DM
			REMOVE_STREAM	//! Usuwamy strumieñ z DM
		};

		//! Obiekt opisuj¹cy zmianê w DM
		struct StreamChange
		{
			StreamConstPtr stream;			//! Strumieñ który modyfikujemy
			std::string extension;			//! Rozszerzenie z jakim strumieñ by³ skojarzony
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

		//! WskaŸnik na obiek obserwuj¹cy zmiany
		typedef core::shared_ptr<IStreamObserver> StreamObserverPtr;

	public:
		//! \param streamWatcher Rejestrowany obiekt obserwuj¹cy zmiany managera strumieni
		virtual void addObserver(const StreamObserverPtr & streamWatcher) = 0;
		//! \param streamWatcher Wyrejestrowywany obiekt obserwuj¹cy zmiany managera strumieni
		virtual void removeObserver(const StreamObserverPtr & streamWatcher) = 0;

		//! \param files Zbiór plików ktrymi aktualnie zarz¹dza ten DataManager
		virtual void getStreams(StreamsList & streams) const = 0;

		//! \param file Plik kótry weryfikujemy czy jest zarz¹dzany przez DM
		//! \return Prawda jeœli plik jest zarz¹dzany przez ten DM
		virtual const bool isManaged(const std::ifstream * stream) const = 0;
		//! \param files Zbior plików dla których chcemy pobraæ listê obiektów
		//! \return Mapa obiektów wzglêdem plików z których pochodza
		virtual void getObjects(const std::ifstream * stream, core::ConstObjectsList & objects) const = 0;

		//! \param files Zbior plików dla których chcemy pobraæ listê obiektów
		//! \return Mapa obiektów wzglêdem plików z których pochodza
		virtual void getObjects(const std::ifstream * stream, core::ObjectWrapperCollection & objects) const = 0;
	};

}

#endif	//	HEADER_GUARD___ISTREAMMANAGERREADER_H__
