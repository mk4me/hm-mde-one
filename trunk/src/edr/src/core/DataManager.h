/**
@author Marek Daniluk
@brief Klasa DataManager implementuje interfejs core::IDataManager. S�u�y do zarz�dzania zasobami edytora, kt�re dziel� si� na dane
konfiguracyjne i pr�by pomiarowe. Zasoby �adowane s� z odpowiednich �cie�ek, domy�lnie zasoby sta�e powinny znajdowa� si� w
resources, pr�by pomiarowe s� wyszukiwane i pobierane do trials.
*/
#ifndef HEADER_GUARD_CORE_DATAMANAGER_H__
#define HEADER_GUARD_CORE_DATAMANAGER_H__
#include <core/TypeInfo.h>
#include <core/IParser.h>
#include <core/ObjectWrapperFactory.h>
#include <core/IDataManager.h>

//#include <core/IPath.h>
//#include <utils/Utils.h>
#include <utils/signalslib.hpp>


class DataManager: public core::IDataManager
{
public:
	DataManager();
	virtual ~DataManager();

public:

	virtual void clear();

	virtual const std::string& getApplicationSkinsFilePath(int i);
	virtual int getApplicationSkinsFilePathCount();

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


public:
	//! Opis pojedynczego rozszerzenia plik�w
	struct ExtensionDescription
	{
		//! Opis rozszerzenia
		std::string description;
		//! Mapa mo�liwych typ�w i ich nazw, kt�re mo�an wyci�gn�c z tego rozszerzenia, to czy w konretnym pliku si� pojawi� czy nie to inna sprawa
		//! Zapytanie takie mozna robi� bez parsowania w oparciu o rejestrowane prototypy parser�w dla danych rozszerze�
		//! Info czy dany typ wyst�pi� w konretnym pliku czy nie wymaga parsowania i s�u�y do tego specjalna funkcja
		std::map<core::TypeInfo, std::string> possibleTypes;
	};

	//! S�ownik rozszerze� i ich opis�w.
	typedef std::map<std::string, ExtensionDescription> ExtensionDescriptions;

    typedef core::Filesystem::Path Path;

private:
	//! Deklaracja wewn�trznej reprezentacji parsera, obudowauj�cej core::IParser
	class Parser;
	//! Deklaracja predykatu. Zagnie�d�ony, aby mie� dost�p do typu Parser.
	struct FindByFilenamePredicate;
	//! Deklaracja predykatu. Zagnie�d�ony, aby mie� dost�p do typu Parser.
	struct FindByRelativePathPredicate;

	//! Wska�nik na parser.
	typedef core::shared_ptr<Parser> ParserPtr;
	//! S�aby wska�nik na parser
	typedef core::weak_ptr<Parser> ParserWeakPtr;
	// TODO: zastanwoi� si� nad u�yciem s�abych wska�nik�w

private:
	//! Wpis s�ownika obiekt�w.
	struct ObjectsMapEntry {
		//! Obiekt.
		core::ObjectWrapperPtr object;
		//! Parser z kt�rego dany obiekt pochodzi.
		ParserPtr parser;
	};

	//! S�ownik parser�w wed�ug ID.
	typedef std::map<UniqueID, core::IParserPtr> IParsersByID;
	//! Multis�ownik parser�w.
	typedef std::multimap<std::string, core::IParserPtr> IParsersByExtensions;
	//! Sekwencja parser�w.
	typedef std::vector<ParserPtr> ParsersList;
	//! Typ mapy obiekt�w.
	typedef std::multimap< core::TypeInfo, ObjectsMapEntry > ObjectsByType;
	//! Mapa fabryk obiekt�w.
	typedef std::map<core::TypeInfo, core::IObjectWrapperFactoryPtr> ObjectFactories;
    //! Multimapa obiekt�w z element�w przetwarzaj�cych
	typedef std::multimap< core::TypeInfo, std::pair<DataProcessorPtr, core::ObjectWrapperWeakPtr> > ObjectFromProcessors;
    //! Mapa typ�w i prototyp�w ich ObjectWrapper�w
	typedef std::map<core::TypeInfo, core::ObjectWrapperConstPtr> RegisteredTypesPrototypes;
    //! Mapa surowych wska�nik�w i odpowiadaj�cych im ObjectWrapper�w
    typedef std::map<void *, core::ObjectWrapperPtr> RawObjectWrapperMapping;
    //! Typ opisuj�cy kolekcj� dancyh wprowadzanych do DataManagera z zewn�trz
    typedef std::set<core::ObjectWrapperPtr> ObjectWrapperSet;

    typedef std::map<core::TypeInfo, ObjectWrapperSet> TypedExternalData;

private:
	//! S�ownik fabryk typ�w.
	ObjectFactories objectFactories;
	//! S�owniki parser�w niezainicjalizowanych.
	IParsersByID registeredParsers;
	//! S�ownik parser�w przypisanych do rozszerze�.
	IParsersByExtensions registeredExtensions;
	//! S�ownik opisu rozszerze� zarejestrowanych przez parsery
	ExtensionDescriptions extensionDescriptions;

	//! Obiekty pochodz�ce z parser�w.
	ObjectsByType currentObjects;
    //! Dane z DataProcesor�w
	ObjectFromProcessors objectsFromDataProcessors;

	//! Lista parser�w przypisanych do plik�w.
	ParsersList currentParsers;

	//! Lista zasob�w.
	std::vector<std::string> resourcesPaths;
	//! Lista sk�rek dla UI
	std::vector<std::string> applicationSkinsPaths;

    //! Prototypy ObjecWrapper�w zarejestrowanych typ�w danych
	RegisteredTypesPrototypes registeredTypesPrototypes;

    //! Mapowanie surowcyh wska�nik�w do ObjectWrapper�w - u�ywane przy obs�udze wymiany danych w Workflow
    RawObjectWrapperMapping rawPointerToObjectWrapper;
    //! Dane wprowadzane z zewn�trz do DataManagera (nie przez Parsery)
    ObjectWrapperSet externalData;
    //! Dane wprowadzone z zewn�trz pogrupowane wg typ�w
    TypedExternalData groupedExternalData;

	//boost::signal<void (const core::Filesystem::Path&, bool)> fileLoadedSignal;
	boost::signal<void (const Path&, const std::vector<core::ObjectWrapperPtr>&, bool)> wrappersAddedSignal;

protected:
	//! Pomocnicza metoda, tworzy parsery dla zadanego rozszerzenia. Wspiera dodawanie wielu parser�w dla jednego pliku.
	//! Uwaga: plik nie zostaje jeszcze przeparsowany!
	//! Zwraca obiekty domenowe ktore moga powstac w wyniku parsowania danego pliku!! jeszcze nie sa zainicjowane
	std::vector<core::ObjectWrapperPtr> createParsers( const Path& path, bool resource );

	//! \param �cie�ka do folderu z plikami pr�by pomiarowej
	//! \return Pojedyncza pr�ba pomiarowa ze �cie�kami do wszystkich jej plik�w.
	LocalTrial findLocalTrialsPaths(const Path& path);


public:
	static DataManager* getInstance()
	{
		return static_cast<DataManager*>(core::getDataManager());
	}

    //! \param factory Fabryka ObjectWrapper�w zadanego typu dostarczana wraz z nowym typem rejestorwanym w aplikacji
	void registerObjectFactory(const core::IObjectWrapperFactoryPtr & factory);

    //! \param type Typ dla kt�rego chcemy utworzy� ObjectWrapper
    //! \return ObjectWrapper dla zadanego typu
	virtual core::ObjectWrapperPtr createWrapper(const core::TypeInfo& type);

    //! \param type Typ dla kt�rego chcemy utworzy� ObjectWrapperCollection
    //! \return ObjectWrapperCollection dla zadanego typu
	virtual core::ObjectWrapperCollectionPtr createWrapperCollection(const core::TypeInfo& typeInfo);

    //! \param type Typ dla kt�rego chcemy dosta� prototyp ObjectWrappera
    //! \return prototyp ObjectWrapper dla zadanego typu
	const core::ObjectWrapperConstPtr & getTypePrototype(const core::TypeInfo & typeInfo) const;

	//! \param sourceTypeInfo Typ z ktorego chcemy pobrac dane
	//! \param destTypeInfo Typ do ktorego chcemy zapisac dane
	//! \return true jesli typ �r�d�owy wspiera typ docelowy lub sa identyczne
	bool isTypeCompatible(const core::TypeInfo & sourceTypeInfo, const core::TypeInfo & destTypeInfo) const
	{
		if(sourceTypeInfo == destTypeInfo){
			return true;
		}

		bool ret = false;
		core::ObjectWrapper::Types supportedTypes;
		getTypePrototype(sourceTypeInfo)->getSupportedTypes(supportedTypes);
		if(std::find(supportedTypes.begin(), supportedTypes.end(), destTypeInfo) != supportedTypes.end()){
			ret = true;
		}

		return ret;
	}

	const ExtensionDescriptions & getRegisteredExtensionsWithDescriptions() const
	{
		return extensionDescriptions;
	}

	//! Rejestruje zadany parser.
	//! \param newService
	void registerParser(const core::IParserPtr & parser);
	//! \return Liczba niezainicjalizowanych parser�w.
	int getNumRegisteredParsers() const;
	//! \param idx Indeks parsera.
	//! \return Parser o zadanym indeksie. Parser zawsze b�dzie niezainicjowany.
	core::IParserConstPtr getRegisteredParser(int idx) const;

	// core::IDataManager
public:
    virtual core::ObjectWrapperPtr getWrapper(void * rawPtr) const;

    //! \param object ObjectWrapperPtr z nowymi danymi domenowymi wytworzonymi w czasie dzia�ania programu
    virtual void addExternalData(const core::ObjectWrapperPtr & object);

    //! \param object ObjectWrapperPtr do usuniecia wraz z danymi domenowymi, DataManager juz wiecej tych danych nie zwroci przy zapytaniu pasujacemu ich typowi
    virtual void removeExternalData(const core::ObjectWrapperPtr & object);

	//! Tutaj nast�puje leniwa inicjalizacja.
	//! \see core::IDataManager::getObjects
	virtual void getObjects(std::vector<core::ObjectWrapperPtr>& objects, const core::TypeInfo& type, bool exact = false);
	virtual void getObjectsFromParsers(std::vector<core::ObjectWrapperPtr>& objects, const core::TypeInfo& type, bool exact = false);
    virtual void getExternalData(std::vector<core::ObjectWrapperPtr>& objects, const core::TypeInfo& type, bool exact = false);
	//! \see core::IDataManager::isExtensionSupported
	virtual bool isExtensionSupported(const std::string& extension) const;

	//! Dodawanie obiektow domenowych z innych zrodel niz parsery
	//! \param dataProcessor zrodlo danych
	//! \param objects wektor ze stworzonymi obiektami domenowymi (zrodlo powinno dalej je przechowywac!)
	virtual void addObjects(const DataProcessorPtr & dataProcessor, const std::vector<core::ObjectWrapperPtr>& objects);

	//! Dodawanie obiektu domenowego z innego zrodla niz parser
	//! \param dataProcessor zrodlo obiektu
	//! \param object dodawany obiekt (zrodlo powinno dalej go przechowywac)
	virtual void addObject(const DataProcessorPtr & dataProcessor, const core::ObjectWrapperPtr & object);

	//! Szuka na dysku zasob�w.
	virtual void findResources(const std::string& resourcesPath);
	//! \param files lista do za�adowania, inicjalizacja parser�w
	virtual void loadFiles(const std::vector<Path>& files) {
		core::ObjectWrapper::Types t;
		loadFiles(files, t);
	}
	//! \param files lista do za�adowania, inicjalizacja parser�w
	virtual void loadFiles(const std::vector<Path>& files, const core::ObjectWrapper::Types& types)
	{
		std::vector<core::ObjectWrapperPtr> objects;
		loadFiles(files, types, objects);
	}

	void loadFiles(const std::vector<Path>& files, const core::ObjectWrapper::Types& types, std::vector<core::ObjectWrapperPtr> & objects);
	void loadFiles(const std::vector<Path>& files, std::vector<core::ObjectWrapperPtr> & objects)
	{
		core::ObjectWrapper::Types t;
		loadFiles(files, t, objects);
	}

	//! �adowanie zasob�w, czyli inicjalizacja parser�w
	virtual void loadResources();

	virtual void addFiles(const std::vector<Path>& files);
	virtual void removeFiles(const std::vector<Path>& files);

	//! Czyszczenie po parserach.
	void clearParsers();

	//virtual void addFileCallback(boost::function<void (const core::Filesystem::Path&, bool)> function);
	//virtual void removeFileCallback(boost::function<void (const core::Filesystem::Path&, bool)> function);
	virtual void addWrappersCallback(boost::function<void (const core::Filesystem::Path&, const std::vector<core::ObjectWrapperPtr>&, bool)> function);
	virtual void removeWrappersCallback(boost::function<void (const core::Filesystem::Path&, const std::vector<core::ObjectWrapperPtr>&, bool)> function);
	//! przeparsowanie konkretnego wrappera
	virtual bool tryParseWrapper(core::ObjectWrapperPtr wrapper);

private:
	//! Mapuje obiekty 
	void mapObjectsToTypes(const std::vector<core::ObjectWrapperPtr>& objects, const ParserPtr & parser );
	//! Usuwa obiekty kieruj�c si� jakimi� wyznacznikami.
	template <class Predicate>
	void removeObjects( Predicate pred );

	void dropRemovedWrappers(ObjectFromProcessors& objectsToCheck);
};

#endif // DATA_MANAGER_H
