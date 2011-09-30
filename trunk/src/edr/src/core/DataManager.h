/**
@author Marek Daniluk
@brief Klasa DataManager implementuje interfejs core::IDataManager. S³u¿y do zarz¹dzania zasobami edytora, które dziel¹ siê na dane
konfiguracyjne i próby pomiarowe. Zasoby ³adowane s¹ z odpowiednich œcie¿ek, domyœlnie zasoby sta³e powinny znajdowaæ siê w
resources, próby pomiarowe s¹ wyszukiwane i pobierane do trials.
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
	//! Opis pojedynczego rozszerzenia plików
	struct ExtensionDescription
	{
		//! Opis rozszerzenia
		std::string description;
		//! Mapa mo¿liwych typów i ich nazw, które mo¿an wyci¹gn¹c z tego rozszerzenia, to czy w konretnym pliku siê pojawi¹ czy nie to inna sprawa
		//! Zapytanie takie mozna robiæ bez parsowania w oparciu o rejestrowane prototypy parserów dla danych rozszerzeñ
		//! Info czy dany typ wyst¹pi³ w konretnym pliku czy nie wymaga parsowania i s³u¿y do tego specjalna funkcja
		std::map<core::TypeInfo, std::string> possibleTypes;
	};

	//! S³ownik rozszerzeñ i ich opisów.
	typedef std::map<std::string, ExtensionDescription> ExtensionDescriptions;

    typedef core::Filesystem::Path Path;

private:
	//! Deklaracja wewnêtrznej reprezentacji parsera, obudowauj¹cej core::IParser
	class Parser;
	//! Deklaracja predykatu. Zagnie¿d¿ony, aby mieæ dostêp do typu Parser.
	struct FindByFilenamePredicate;
	//! Deklaracja predykatu. Zagnie¿d¿ony, aby mieæ dostêp do typu Parser.
	struct FindByRelativePathPredicate;

	//! WskaŸnik na parser.
	typedef core::shared_ptr<Parser> ParserPtr;
	//! S³aby wskaŸnik na parser
	typedef core::weak_ptr<Parser> ParserWeakPtr;
	// TODO: zastanwoiæ siê nad u¿yciem s³abych wskaŸników

private:
	//! Wpis s³ownika obiektów.
	struct ObjectsMapEntry {
		//! Obiekt.
		core::ObjectWrapperPtr object;
		//! Parser z którego dany obiekt pochodzi.
		ParserPtr parser;
	};

	//! S³ownik parserów wed³ug ID.
	typedef std::map<UniqueID, core::IParserPtr> IParsersByID;
	//! Multis³ownik parserów.
	typedef std::multimap<std::string, core::IParserPtr> IParsersByExtensions;
	//! Sekwencja parserów.
	typedef std::vector<ParserPtr> ParsersList;
	//! Typ mapy obiektów.
	typedef std::multimap< core::TypeInfo, ObjectsMapEntry > ObjectsByType;
	//! Mapa fabryk obiektów.
	typedef std::map<core::TypeInfo, core::IObjectWrapperFactoryPtr> ObjectFactories;
    //! Multimapa obiektów z elementów przetwarzaj¹cych
	typedef std::multimap< core::TypeInfo, std::pair<DataProcessorPtr, core::ObjectWrapperWeakPtr> > ObjectFromProcessors;
    //! Mapa typów i prototypów ich ObjectWrapperów
	typedef std::map<core::TypeInfo, core::ObjectWrapperConstPtr> RegisteredTypesPrototypes;
    //! Mapa surowych wskaŸników i odpowiadaj¹cych im ObjectWrapperów
    typedef std::map<void *, core::ObjectWrapperPtr> RawObjectWrapperMapping;
    //! Typ opisuj¹cy kolekcjê dancyh wprowadzanych do DataManagera z zewn¹trz
    typedef std::set<core::ObjectWrapperPtr> ObjectWrapperSet;

    typedef std::map<core::TypeInfo, ObjectWrapperSet> TypedExternalData;

private:
	//! S³ownik fabryk typów.
	ObjectFactories objectFactories;
	//! S³owniki parserów niezainicjalizowanych.
	IParsersByID registeredParsers;
	//! S³ownik parserów przypisanych do rozszerzeñ.
	IParsersByExtensions registeredExtensions;
	//! S³ownik opisu rozszerzeñ zarejestrowanych przez parsery
	ExtensionDescriptions extensionDescriptions;

	//! Obiekty pochodz¹ce z parserów.
	ObjectsByType currentObjects;
    //! Dane z DataProcesorów
	ObjectFromProcessors objectsFromDataProcessors;

	//! Lista parserów przypisanych do plików.
	ParsersList currentParsers;

	//! Lista zasobów.
	std::vector<std::string> resourcesPaths;
	//! Lista skórek dla UI
	std::vector<std::string> applicationSkinsPaths;

    //! Prototypy ObjecWrapperów zarejestrowanych typów danych
	RegisteredTypesPrototypes registeredTypesPrototypes;

    //! Mapowanie surowcyh wskaŸników do ObjectWrapperów - u¿ywane przy obs³udze wymiany danych w Workflow
    RawObjectWrapperMapping rawPointerToObjectWrapper;
    //! Dane wprowadzane z zewn¹trz do DataManagera (nie przez Parsery)
    ObjectWrapperSet externalData;
    //! Dane wprowadzone z zewn¹trz pogrupowane wg typów
    TypedExternalData groupedExternalData;

	//boost::signal<void (const core::Filesystem::Path&, bool)> fileLoadedSignal;
	boost::signal<void (const Path&, const std::vector<core::ObjectWrapperPtr>&, bool)> wrappersAddedSignal;

protected:
	//! Pomocnicza metoda, tworzy parsery dla zadanego rozszerzenia. Wspiera dodawanie wielu parserów dla jednego pliku.
	//! Uwaga: plik nie zostaje jeszcze przeparsowany!
	//! Zwraca obiekty domenowe ktore moga powstac w wyniku parsowania danego pliku!! jeszcze nie sa zainicjowane
	std::vector<core::ObjectWrapperPtr> createParsers( const Path& path, bool resource );

	//! \param œcie¿ka do folderu z plikami próby pomiarowej
	//! \return Pojedyncza próba pomiarowa ze œcie¿kami do wszystkich jej plików.
	LocalTrial findLocalTrialsPaths(const Path& path);


public:
	static DataManager* getInstance()
	{
		return static_cast<DataManager*>(core::getDataManager());
	}

    //! \param factory Fabryka ObjectWrapperów zadanego typu dostarczana wraz z nowym typem rejestorwanym w aplikacji
	void registerObjectFactory(const core::IObjectWrapperFactoryPtr & factory);

    //! \param type Typ dla którego chcemy utworzyæ ObjectWrapper
    //! \return ObjectWrapper dla zadanego typu
	virtual core::ObjectWrapperPtr createWrapper(const core::TypeInfo& type);

    //! \param type Typ dla którego chcemy utworzyæ ObjectWrapperCollection
    //! \return ObjectWrapperCollection dla zadanego typu
	virtual core::ObjectWrapperCollectionPtr createWrapperCollection(const core::TypeInfo& typeInfo);

    //! \param type Typ dla którego chcemy dostaæ prototyp ObjectWrappera
    //! \return prototyp ObjectWrapper dla zadanego typu
	const core::ObjectWrapperConstPtr & getTypePrototype(const core::TypeInfo & typeInfo) const;

	//! \param sourceTypeInfo Typ z ktorego chcemy pobrac dane
	//! \param destTypeInfo Typ do ktorego chcemy zapisac dane
	//! \return true jesli typ Ÿród³owy wspiera typ docelowy lub sa identyczne
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
	//! \return Liczba niezainicjalizowanych parserów.
	int getNumRegisteredParsers() const;
	//! \param idx Indeks parsera.
	//! \return Parser o zadanym indeksie. Parser zawsze bêdzie niezainicjowany.
	core::IParserConstPtr getRegisteredParser(int idx) const;

	// core::IDataManager
public:
    virtual core::ObjectWrapperPtr getWrapper(void * rawPtr) const;

    //! \param object ObjectWrapperPtr z nowymi danymi domenowymi wytworzonymi w czasie dzia³ania programu
    virtual void addExternalData(const core::ObjectWrapperPtr & object);

    //! \param object ObjectWrapperPtr do usuniecia wraz z danymi domenowymi, DataManager juz wiecej tych danych nie zwroci przy zapytaniu pasujacemu ich typowi
    virtual void removeExternalData(const core::ObjectWrapperPtr & object);

	//! Tutaj nastêpuje leniwa inicjalizacja.
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

	//! Szuka na dysku zasobów.
	virtual void findResources(const std::string& resourcesPath);
	//! \param files lista do za³adowania, inicjalizacja parserów
	virtual void loadFiles(const std::vector<Path>& files) {
		core::ObjectWrapper::Types t;
		loadFiles(files, t);
	}
	//! \param files lista do za³adowania, inicjalizacja parserów
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

	//! ³adowanie zasobów, czyli inicjalizacja parserów
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
	//! Usuwa obiekty kieruj¹c siê jakimiœ wyznacznikami.
	template <class Predicate>
	void removeObjects( Predicate pred );

	void dropRemovedWrappers(ObjectFromProcessors& objectsToCheck);
};

#endif // DATA_MANAGER_H
