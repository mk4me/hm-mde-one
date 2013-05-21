/**
@author Marek Daniluk
@brief Klasa DataManager implementuje interfejs core::IDataManager. Służy do zarządzania zasobami edytora, które dzielą się na dane
konfiguracyjne i próby pomiarowe. Zasoby ładowane są z odpowiednich ścieżek, domyślnie zasoby stałe powinny znajdować się w
resources, próby pomiarowe są wyszukiwane i pobierane do trials.
*/
#ifndef HEADER_GUARD_CORE_DATAMANAGER_H__
#define HEADER_GUARD_CORE_DATAMANAGER_H__

#include <OpenThreads/ReentrantMutex>
#include <OpenThreads/ScopedLock>
#include <core/TypeInfo.h>
#include <core/IParser.h>
#include <core/ObjectWrapper.h>
#include <core/ObjectWrapperFactory.h>
#include <core/IDataManager.h>
#include "ManagerHelper.h"


class DataManager : public core::IDataManagerReader, public core::IMemoryDataManager,
    public core::IFileDataManager, public ManagerHelper<DataManager>
{
public:
    DataManager();
    virtual ~DataManager();

private:
    //! Deklaracja wewnętrznej reprezentacji parsera, obudowaującej core::IParser
    class Parser;

    //! Wskaźnik na parser.
    typedef core::shared_ptr<Parser> ParserPtr;
    //! Słaby wskaźnik na parser
    typedef core::weak_ptr<Parser> ParserWeakPtr;
    // TODO: zastanwoić się nad użyciem słabych wskaźników


    typedef OpenThreads::ScopedLock<OpenThreads::ReentrantMutex> ScopedLock;

private:
    
	void initializeDataWithParser(core::ObjectWrapper & object, const ParserPtr & parser);

    //! ----------------------------- Dane stałe w Datamanager ---------------------------------

    //! Dane stałe na temat rozszerzenia
    struct ExtendedExtensionDescription : public ExtensionDescription
    {
        //! Prototypu parserów wspierające to rozszerzenie
        std::set<core::IParserPtr> parsers;
    };

    //! Kolekcja parserów zarządzanych przez DataManager
    typedef std::set<ParserPtr> Parsers;

    //! Słownik prototypów parserów według ID.
    typedef std::map<UniqueID, core::IParserPtr> IParsersByID;

    //! Słownik wspieranych przez aplikacje rozszerzeń plików poprzez mechanizm parserów
    typedef std::map<std::string, ExtendedExtensionDescription> SupportedExtensionsPersistenceData;

    //! ---------------------------------------------------------------------------------

    //! -------------------- Struktury do obsługi danych pod postacia ObjectWrapperów i Parserów ------------------

    //! Słownik prawdziwych parserów i związanych z nimi obiektów, które można pobrać z DataManagera
    typedef std::map<ParserPtr, core::Objects> ObjectsByParsers;
    //! Słownik obiektów domenowych załadowanych do DataManger i odpowiadających im parserów
    typedef std::map<core::ObjectWrapperPtr, ParserPtr> ParsersByObjects;
    //! Słownik aktualnie obsługiwanych plików i skojarzonych z nimi parserów
    typedef std::map<core::Filesystem::Path, Parsers> ParsersByFiles;
    //! Typ mapy obiektów.
    typedef std::map< core::TypeInfo, core::Objects > ObjectsByTypes;

    //! ---------------------------------------------------------------------------------

    //! Mapa typów i prototypów ich ObjectWrapperów
    typedef std::map<core::TypeInfo, core::ObjectWrapperConstPtr> RegisteredTypesPrototypes;
    //! Słownik hierarchii typow -> mapuje typ do jego typow bazowych (hierarchia dziedziczenia) [first] i do typów po nim dziedziczacych [second]
    typedef std::map<core::TypeInfo, std::pair<core::TypeInfoSet, core::TypeInfoSet> > TypesHierarchy;

private:
    //! Słowniki parserów niezainicjalizowanych.
    IParsersByID registeredParsers;

    //! Słownik parserów przypisanych do rozszerzeń.
    SupportedExtensionsPersistenceData registeredExtensions;

    //! Zarejestrowane rozszerzenia plików
    Extensions extensions;

    //! Zbiór zarządzanych przez DM ObjectWrapperów
    core::Objects objects;

    //! Mapa obiektów wg parserów
    ObjectsByParsers objectsByParsers;

    //! Mapa parserów wg obiektów
    ParsersByObjects parsersByObjects;

    //! Mapa parserów wg plików
    ParsersByFiles parsersByFiles;

    //! Obiekty pochodzące z parserów.
    ObjectsByTypes objectsByTypes;

    //! Prototypy ObjecWrapperów zarejestrowanych typów danych
    RegisteredTypesPrototypes registeredTypesPrototypes;

    //! Zarejestrowane w aplikacji typy danych
    core::Types registeredTypes;

    //! Hierarchia zaresjetrowanych typów danych
    TypesHierarchy typesHierarchy;

    //! Synchronizacja operacji na DM
    mutable OpenThreads::ReentrantMutex stateMutex;

public:
    //! \param extension Rozszerzenie, które doprowadzamy do formy obsługiwanej przez DM - zaczynamy od kropki, wszystko małymi literami
    static void prepareExtension(std::string & extension);

    //! \param factory Fabryka ObjectWrapperów zadanego typu dostarczana wraz z nowym typem rejestorwanym w aplikacji
    void registerObjectWrapperPrototype(const core::ObjectWrapperConstPtr & prototype);

    //! \param type Typ dla którego chcemy utworzyć ObjectWrapperCollection
    //! \return ObjectWrapperCollection dla zadanego typu
    virtual core::ObjectWrapperCollectionPtr createWrapperCollection(const core::TypeInfo& typeInfo);

    //! \param type Typ dla którego chcemy dostać prototyp ObjectWrappera
    //! \return prototyp ObjectWrapper dla zadanego typu
    const core::ObjectWrapperConstPtr & getTypePrototype(const core::TypeInfo & typeInfo) const;

    ////! \param sourceTypeInfo Typ z którego chcemy pobrać dane
    ////! \param destTypeInfo Typ do którego chcemy zapisac dane
    ////! \return true jeśli typ źródłowy wspiera typ docelowy lub są identyczne
    bool isTypeCompatible(const core::TypeInfo & sourceTypeInfo, const core::TypeInfo & destTypeInfo) const;

    //! Rejestruje żądany parser.
    //! \param newService
    void registerParser(const core::IParserPtr & parser);
    //! \return Liczba niezainicjalizowanych parserów.
    int getNumRegisteredParsers() const;
    //! \param idx Indeks parsera.
    //! \return Parser o zadanym indeksie. Parser zawsze będzie niezainicjowany.
    core::IParserConstPtr getRegisteredParser(int idx) const;

//IDataManagerReader
public:

    virtual void getObjects(std::vector<core::ObjectWrapperConstPtr>& objects, const core::TypeInfo& type, bool exact = true);

    virtual void getObjects(core::ObjectWrapperCollection& objects);

    //! \return Zarejestrowane w aplikacji typy danych
    virtual const core::TypeInfoSet & getSupportedTypes() const;

    //! \return Hierarchia typów danych - jakie operacje moge realizować, po czym dziedzicze
    virtual const core::TypeInfoSet & getTypeBaseTypes(const core::TypeInfo & type) const;

    //! \return Hierarchia typów danych - jakie typy po mnie dziedzicza, kto wspiera moj interfejs i może być downcastowany na mnie
    virtual const core::TypeInfoSet & getTypeDerrivedTypes(const core::TypeInfo & type) const;

    //core::IMemoryDataManager
public:

    //! \param objects Zbiór obiektów domenowych zarządzanych przez ten DataManager
    virtual void getManagedData(core::Objects & objects) const;

private:

	void removeDataImpl(const core::ObjectWrapperPtr & data);

	//! \param Obiekt który zostanie usunięty jeśli zarządza nim DataManager
	virtual void nonNotifyRemoveData(const core::ObjectWrapperPtr & data);

    //! \param Obiekt który zostanie utrwalony w DataManager i będzie dostępny przy zapytaniach, nie może być niezainicjowany - isNull musi być false!!
    virtual void nonNotifyAddData(const core::ObjectWrapperPtr & data, const core::ObjectWrapper::LazyInitializer & initializer = core::ObjectWrapper::LazyInitializer());

    // core::IFileDataManager
public:

    //! \param files Zbiór plików ktrymi aktualnie zarządza ten DataManager
    virtual void getManagedFiles(core::Files & files) const;

    virtual const bool isFileManaged(const core::Filesystem::Path & file) const;

    //! \param files Zbior plików dla których chcemy pobrać liste obiektów
    //! \return Mapa obiektów względem plików z których pochodza
    virtual void getObjectsForFile(const core::Filesystem::Path & file, std::vector<core::ObjectWrapperPtr> & objects) const;

    //! \return true jeśli rozszerznie jest wspierane przez DataManager, w przeciwnym wypadku false
    virtual bool isExtensionSupported(const std::string & extension) const;

    //! \return Zbior obsługiwanych rozszerzen plików wraz z ich opisem
    virtual const Extensions & getSupportedFilesExtensions() const;

    virtual const ExtensionDescription & getExtensionDescription(const std::string & extension) const;

private:

	//! \param files Lista plików dla których zostaną utworzone parsery i z których wyciągnięte dane
	//! będa dostępne poprzez DataMangera LENIWA INICJALIZACJA
	virtual void nonNotifyAddFile(const core::Filesystem::Path & file, std::vector<core::ObjectWrapperPtr> & objects);

	//! \param files Lista plików które zostaną usunięte z aplikacji a wraz z nimi skojarzone parsery i dane
	virtual void nonNotifyRemoveFile(const core::Filesystem::Path & file);

};

#endif // DATA_MANAGER_H
