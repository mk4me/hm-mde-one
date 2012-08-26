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


class DataManager : public virtual core::IDataManagerReader, public core::IMemoryDataManager,
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

    class DMObjectWrapper : public core::IObjectWrapper, public core::enable_shared_from_this<DMObjectWrapper>
    {
    public:
        DMObjectWrapper(const core::ObjectWrapperPtr & wrapper);
        ~DMObjectWrapper();

        virtual const void* getRawPtr() const;
        virtual void* getRawPtr();
        virtual void reset();


        //! \return ID typu.
        virtual std::size_t getClassID() const;
        //! \return Nazwa typu.
        virtual const std::string& getClassName() const;
        //! \return Nazwa obiektu.
        virtual const std::string& getName() const;
        //! \param name Nazwa obiektu.
        virtual void setName(const std::string& name);
        //! \return
        virtual const std::string& getSource() const;
        //! \param source
        virtual void setSource(const std::string& source);

        //! \return
        virtual bool isChanged() const;
        //! \param changed
        virtual void setChanged(bool changed);

        //! \param type
        //! \return Czy obiekt wspiera określony typ?
        virtual bool isSupported(const core::TypeInfo& type) const;
        //! \param type Typ inteligentnego wskaźnika.
        //! \return true jeżeli do z obiektu można wyłuskać dany wskaźnik.
        virtual bool isPtrSupported(const core::TypeInfo& type) const;

        //! \return Informacje o typie.
        virtual core::TypeInfo getTypeInfo() const;

        //! \return Informacje o typie odpowiednio normalnego i stałego wskaźnika.
        virtual std::pair<core::TypeInfo, core::TypeInfo> getPtrTypeInfo() const;

        //! \param supported Lista wspieranych rozszerzeń.
        virtual void getSupportedTypes(Types& supported) const;

        //! \return Czy wrappowany obiekt jest wyzerowany?
        virtual bool isNull() const;

        //! \return Klon bieżącego obiektu. Wewnętrzny wskaźnik również jest kopiowany.
        virtual core::ObjectWrapperPtr clone() const;

        virtual core::ObjectWrapperPtr create() const;

    protected:

        virtual bool setSmartPtr(const void* ptr, const core::TypeInfo& type);

        //! \return Czy udało się ustawić mądry wskaźnik?
        virtual bool getSmartPtr(void* ptr, const core::TypeInfo& type) const;

    private:

        mutable core::ObjectWrapperPtr wrapper;

    };

    class ParserInitializer : public core::IDataInitializer
    {
    public:
        ParserInitializer(const ParserPtr & parser);

        ~ParserInitializer();


        virtual void initialize(core::ObjectWrapperPtr & object);

    protected:
        virtual void doDeinitialize(core::ObjectWrapperPtr & object);

    private:
        ParserPtr parser;
    };

    friend class ParserInitializer;

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

    typedef std::map<core::ObjectWrapperPtr, core::DataInitializerPtr> ObjectsWithInitializers;

    //! Słownik prawdziwych parserów i związanych z nimi obiektów, które można pobrać z DataManagera
    typedef std::map<ParserPtr, core::Objects> ObjectsByParsers;
    //! Słownik obiektów domenowych załadowanych do DataManger i odpowiadających im parserów
    typedef std::map<core::ObjectWrapperPtr, ParserPtr> ParsersByObjects;
    //! Słownik aktualnie obsługiwanych plików i skojarzonych z nimi parserów
    typedef std::map<core::Filesystem::Path, Parsers> ParsersByFiles;
    //! Typ mapy obiektów.
    typedef std::map< core::TypeInfo, core::Objects > ObjectsByTypes;
    //! Mapa surowych wskaźników i odpowiadających im ObjectWrapperów
    typedef std::map<const void *, core::ObjectWrapperPtr> RawObjectWrapperMapping;

	//! -------------------- Struktury do obsługi metadany ObjectWrapperów ------------------

	//! Metadane wg OW
	typedef std::map<core::ObjectWrapperPtr, std::vector<core::ObjectWrapperPtr>> MetadataByObjects;

    //! ---------------------------------------------------------------------------------

    //! Mapa fabryk obiektów.
    typedef std::map<core::TypeInfo, core::IObjectWrapperFactoryPtr> ObjectFactories;
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

    //! Mapa obiektów i ich inicjalizatorów
    ObjectsWithInitializers objectsWithInitializers;

    //! Mapa obiektów wg parserów
    ObjectsByParsers objectsByParsers;

    //! Mapa parserów wg obiektów
    ParsersByObjects parsersByObjects;

    //! Mapa parserów wg plików
    ParsersByFiles parsersByFiles;

    //! Obiekty pochodzące z parserów.
    ObjectsByTypes objectsByTypes;

	//! Metadane wg OW
	MetadataByObjects metadataByObjects;

    //! Mapowanie surowcyh wskaźników do ObjectWrapperów - używane przy obsłudze wymiany danych w Workflow
    RawObjectWrapperMapping rawPointerToObjectWrapper;

    //! Słownik fabryk typów.
    ObjectFactories objectFactories;

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
    void registerObjectFactory(const core::IObjectWrapperFactoryPtr & factory);

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

    virtual void getObjects(std::vector<core::ObjectWrapperConstPtr>& objects, const core::TypeInfo& type, bool exact = true, bool initialize = true);

    virtual void getObjects(core::ObjectWrapperCollection& objects, bool initialize = true);

	virtual bool isManaged(const core::ObjectWrapperConstPtr & object) const;

	//! \param object Obiekt dla którego pobieram metadane
	//! \param metadataCollection [out] Kolekcja metadanych dla zadanego obiektu
	virtual void getMetadataForObject(const core::ObjectWrapperConstPtr & object, std::vector<core::ObjectWrapperConstPtr> & metadataCollection) const;
	//! \param typeInfo Typ danych dla których chcemy pobrać wszystkie metadane
	//! \param metadataCollection [out] Kolekcja metadanych dla zadanego obiektu
	//! \param exact Czy mamy dołanczać metadane typów pochodnych
	virtual void getMetadataForType(const core::TypeInfo & typeInfo, std::vector<core::ObjectWrapperConstPtr> & metadataCollection, bool exact = true) const;

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

    //! \param Obiekt który chcemy inicjalizować
    virtual void initializeData(core::ObjectWrapperPtr & data);

    //! \param Obiekt dla którego chcemy wykonać deinicjalizacje
    virtual bool isInitializable(const core::ObjectWrapperPtr & data) const;

    //! \param Obiekt który chcemy deinicjalizować - dalej jest w DataManager ale nie zawiera danych - trzeba potem inicjalizować
    virtual void deinitializeData(core::ObjectWrapperPtr & data);

private:

	void removeDataImpl(const core::ObjectWrapperPtr & data);

	//! \param Obiekt który zostanie usunięty jeśli zarządza nim DataManager
	virtual void nonNotifyRemoveData(const core::ObjectWrapperPtr & data);

    //! \param Obiekt który zostanie utrwalony w DataManager i będzie dostępny przy zapytaniach, nie może być niezainicjowany - isNull musi być false!!
    virtual void nonNotifyAddData(const core::ObjectWrapperPtr & data, const core::DataInitializerPtr & initializer = core::DataInitializerPtr());

    virtual const core::ObjectWrapperPtr & getObjectWrapperForRawPtr(const void * ptr) const;

    virtual bool objectIsManaged(const void * ptr) const;

    virtual core::ObjectWrapperPtr createObjectWrapper(const core::TypeInfo & type) const;

    // core::IFileDataManager
public:

    //! \param files Zbiór plików ktrymi aktualnie zarządza ten DataManager
    virtual void getManagedFiles(core::Files & files) const;

    virtual bool isFileManaged(const core::Filesystem::Path & file) const;

    //! \param path Ściezka pliku który chemy załadować (parsować) WYMUSZAMY PARSOWANIE I INICJALIZACJE
    virtual void initializeFile(const core::Filesystem::Path & file);

    //! \param path Ściezka pliku który chemy załadować (parsować) ZWALNIAMY ZASOBY, ALE ZEZWALAMY PONOWNIE NA LENIWA INICJALIZACJE
    virtual void deinitializeFile(const core::Filesystem::Path & file);

    //! \param files Zbior plików dla których chcemy pobrać liste obiektów
    //! \return Mapa obiektów względem plików z których pochodza
    virtual void  getObjectsForFile(const core::Filesystem::Path & file, std::vector<core::ObjectWrapperPtr> & objects) const;

    //! \return true jeśli rozszerznie jest wspierane przez DataManager, w przeciwnym wypadku false
    virtual bool isExtensionSupported(const std::string & extension) const;

    //! \return Zbior obsługiwanych rozszerzen plików wraz z ich opisem
    virtual const Extensions & getSupportedFilesExtensions() const;

    virtual const ExtensionDescription & getExtensionDescription(const std::string & extension) const;

	virtual void notify(const core::IFileDataManager * dm);

private:

	//! \param files Lista plików dla których zostaną utworzone parsery i z których wyciągnięte dane
	//! będa dostępne poprzez DataMangera LENIWA INICJALIZACJA
	virtual void nonNotifyAddFile(const core::Filesystem::Path & file, std::vector<core::ObjectWrapperPtr> & objects);

	//! \param files Lista plików które zostaną usunięte z aplikacji a wraz z nimi skojarzone parsery i dane
	virtual void nonNotifyRemoveFile(const core::Filesystem::Path & file);

};

#endif // DATA_MANAGER_H
