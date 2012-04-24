/**
@author Marek Daniluk
@brief Klasa DataManager implementuje interfejs core::IDataManager. S³u¿y do zarz¹dzania zasobami edytora, które dziel¹ siê na dane
konfiguracyjne i próby pomiarowe. Zasoby ³adowane s¹ z odpowiednich œcie¿ek, domyœlnie zasoby sta³e powinny znajdowaæ siê w
resources, próby pomiarowe s¹ wyszukiwane i pobierane do trials.
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
    //! Deklaracja wewnêtrznej reprezentacji parsera, obudowauj¹cej core::IParser
    class Parser;

    //! WskaŸnik na parser.
    typedef core::shared_ptr<Parser> ParserPtr;
    //! S³aby wskaŸnik na parser
    typedef core::weak_ptr<Parser> ParserWeakPtr;
    // TODO: zastanwoiæ siê nad u¿yciem s³abych wskaŸników


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
        //! \return Czy obiekt wspiera okreœlony typ?
        virtual bool isSupported(const core::TypeInfo& type) const;
        //! \param type Typ inteligentnego wskaŸnika.
        //! \return true je¿eli do z obiektu mo¿na wy³uskaæ dany wskaŸnik.
        virtual bool isPtrSupported(const core::TypeInfo& type) const;

        //! \return Informacje o typie.
        virtual core::TypeInfo getTypeInfo() const;

        //! \return Informacje o typie odpowiednio normalnego i sta³ego wskaŸnika.
        virtual std::pair<core::TypeInfo, core::TypeInfo> getPtrTypeInfo() const;

        //! \param supported Lista wspieranych rozszerzeñ.
        virtual void getSupportedTypes(Types& supported) const;

        //! \return Czy wrappowany obiekt jest wyzerowany?
        virtual bool isNull() const;

        //! \return Klon bie¿¹cego obiektu. Wewnêtrzny wskaŸnik równie¿ jest kopiowany.
        virtual core::ObjectWrapperPtr clone() const;

        virtual core::ObjectWrapperPtr create() const;

    protected:

        virtual bool setSmartPtr(const void* ptr, const core::TypeInfo& type);

        //! \return Czy uda³o siê ustawiæ m¹dry wskaŸnik?
        virtual bool getSmartPtr(void* ptr, const core::TypeInfo& type) const;

    private:

        core::ObjectWrapperPtr wrapper;

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

    //! ----------------------------- Dane sta³e w Datamanager ---------------------------------

    //! Dane sta³e na temat rozszerzenia
    struct ExtendedExtensionDescription : public ExtensionDescription
    {
        //! Prototypu parserów wspierajace to rozszerzenie
        std::set<core::IParserPtr> parsers;
    };

    //! Kolekcja parserów zarzadzanych przez DataManager
    typedef std::set<ParserPtr> Parsers;

    //! S³ownik prototypów parserów wed³ug ID.
    typedef std::map<UniqueID, core::IParserPtr> IParsersByID;

    //! S³ownik wspieranych przez aplikacje rozszerzeñ plików poprzez mechanizm parserów
    typedef std::map<std::string, ExtendedExtensionDescription> SupportedExtensionsPersistenceData;

    //! ---------------------------------------------------------------------------------

    //! -------------------- Struktury do obs³ugi danych pod postacia ObjectWrapperów i Parserów ------------------

    typedef std::map<core::ObjectWrapperPtr, core::DataInitializerPtr> ObjectsWithInitializers;

    //! S³ownik prawdziwych parserów i zwi¹zanych z nimi obiektów, które mo¿na pobraæ z DataManagera
    typedef std::map<ParserPtr, core::Objects> ObjectsByParsers;
    //! S³ownik obiektów domenowych za³adowanych do DataManger i odpowiadaj¹cych im parserów
    typedef std::map<core::ObjectWrapperPtr, ParserPtr> ParsersByObjects;
    //! S³ownik aktualnie obs³ugiwanych plików i skojarzonych z nimi parserów
    typedef std::map<core::Filesystem::Path, Parsers> ParsersByFiles;
    //! Typ mapy obiektów.
    typedef std::map< core::TypeInfo, core::Objects > ObjectsByTypes;
    //! Mapa surowych wskaŸników i odpowiadaj¹cych im ObjectWrapperów
    typedef std::map<const void *, core::ObjectWrapperPtr> RawObjectWrapperMapping;

	//! -------------------- Struktury do obs³ugi metadany ObjectWrapperów ------------------

	//! Metadane wg OW
	typedef std::map<core::ObjectWrapperPtr, std::vector<core::ObjectWrapperPtr>> MetadataByObjects;

    //! ---------------------------------------------------------------------------------

    //! Mapa fabryk obiektów.
    typedef std::map<core::TypeInfo, core::IObjectWrapperFactoryPtr> ObjectFactories;
    //! Mapa typów i prototypów ich ObjectWrapperów
    typedef std::map<core::TypeInfo, core::ObjectWrapperConstPtr> RegisteredTypesPrototypes;
    //! S³ownik hierarchii typow -> mapuje typ do jego typow bazowych (hierarchia dziedziczenia) [first] i do typow po nim dziedziczacych [second]
    typedef std::map<core::TypeInfo, std::pair<core::TypeInfoSet, core::TypeInfoSet> > TypesHierarchy;

private:
    //! S³owniki parserów niezainicjalizowanych.
    IParsersByID registeredParsers;    

    //! S³ownik parserów przypisanych do rozszerzeñ.
    SupportedExtensionsPersistenceData registeredExtensions;

    //! Zarejestrowane rozszerzenia plików
    Extensions extensions;

    //! Zbiór zarz¹dzanych przez DM ObjectWrapperów
    core::Objects objects;

    //! Mapa obiektów i ich inicjalizatorów
    ObjectsWithInitializers objectsWithInitializers;

    //! Mapa obiektów wg parserów
    ObjectsByParsers objectsByParsers;

    //! Mapa parserów wg obiektów
    ParsersByObjects parsersByObjects;

    //! Mapa parserów wg plików
    ParsersByFiles parsersByFiles;

    //! Obiekty pochodz¹ce z parserów.
    ObjectsByTypes objectsByTypes;

	//! Metadane wg OW
	MetadataByObjects metadataByObjects;

    //! Mapowanie surowcyh wskaŸników do ObjectWrapperów - u¿ywane przy obs³udze wymiany danych w Workflow
    RawObjectWrapperMapping rawPointerToObjectWrapper;

    //! S³ownik fabryk typów.
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
    //! \param extension Rozszerzenie, które doprowadzamy do formy obs³ugiwanej przez DM - zaczynamy od kropki, wszystko ma³ymi literami
    static void prepareExtension(std::string & extension);

    //! \param factory Fabryka ObjectWrapperów zadanego typu dostarczana wraz z nowym typem rejestorwanym w aplikacji
    void registerObjectFactory(const core::IObjectWrapperFactoryPtr & factory);

    //! \param type Typ dla którego chcemy utworzyæ ObjectWrapperCollection
    //! \return ObjectWrapperCollection dla zadanego typu
    virtual core::ObjectWrapperCollectionPtr createWrapperCollection(const core::TypeInfo& typeInfo);

    //! \param type Typ dla którego chcemy dostaæ prototyp ObjectWrappera
    //! \return prototyp ObjectWrapper dla zadanego typu
    const core::ObjectWrapperConstPtr & getTypePrototype(const core::TypeInfo & typeInfo) const;

    ////! \param sourceTypeInfo Typ z ktorego chcemy pobrac dane
    ////! \param destTypeInfo Typ do ktorego chcemy zapisac dane
    ////! \return true jesli typ Ÿród³owy wspiera typ docelowy lub sa identyczne
    bool isTypeCompatible(const core::TypeInfo & sourceTypeInfo, const core::TypeInfo & destTypeInfo) const;

    //! Rejestruje zadany parser.
    //! \param newService
    void registerParser(const core::IParserPtr & parser);
    //! \return Liczba niezainicjalizowanych parserów.
    int getNumRegisteredParsers() const;
    //! \param idx Indeks parsera.
    //! \return Parser o zadanym indeksie. Parser zawsze bêdzie niezainicjowany.
    core::IParserConstPtr getRegisteredParser(int idx) const;

//IDataManagerReader
public:

    virtual void getObjects(std::vector<core::ObjectWrapperConstPtr>& objects, const core::TypeInfo& type, bool exact = true);

    virtual void getObjects(core::ObjectWrapperCollection& objects);

	virtual bool isManaged(const core::ObjectWrapperConstPtr & object) const;

	//! \param object Obiekt dla którego pobieram metadane
	//! \param metadataCollection [out] Kolekcja metadanych dla zadanego obiektu
	virtual void getMetadataForObject(const core::ObjectWrapperConstPtr & object, std::vector<core::ObjectWrapperConstPtr> & metadataCollection) const;
	//! \param typeInfo Typ danych dla których chcemy pobrac wszystkie metadane
	//! \param metadataCollection [out] Kolekcja metadanych dla zadanego obiektu
	//! \param exact Czy mamy do³anczaæ metadane typów pochodnych
	virtual void getMetadataForType(const core::TypeInfo & typeInfo, std::vector<core::ObjectWrapperConstPtr> & metadataCollection, bool exact = true) const;

    //! \return Zarejestrowane w aplikacji typy danych
    virtual const core::TypeInfoSet & getSupportedTypes() const;

    //! \return Hierarchia typow danych - jakie operacje moge realizowac, po czym dziedzicze
    virtual const core::TypeInfoSet & getTypeBaseTypes(const core::TypeInfo & type) const;

    //! \return Hierarchia typow danych - jakie typy po mnie dziedzicza, kto wspiera moj interfejs i moze byc downcastowany na mnie
    virtual const core::TypeInfoSet & getTypeDerrivedTypes(const core::TypeInfo & type) const;

    //core::IMemoryDataManager
public:

    //! \param objects Zbiór obiektów domenowych zarz¹dzanych przez ten DataManager
    virtual void getManagedData(core::Objects & objects) const;

    //! \param Obiekt ktory chcemy inicjalizowaæ
    virtual void initializeData(core::ObjectWrapperPtr & data);

    //! \param Obiekt dla ktorego chcemy wykonac deinicjalizacje
    virtual bool isInitializable(const core::ObjectWrapperPtr & data) const;

    //! \param Obiekt ktory chcemy deinicjalizowaæ - dalej jest w DataManager ale nie zawiera danych - trzeba potem inicjalizowaæ
    virtual void deinitializeData(core::ObjectWrapperPtr & data);

    //! \param Obiekt ktory zostanie usuniety jesli zarzadza nim DataManager
    virtual void removeData(const core::ObjectWrapperPtr & data);

private:

    //! \param Obiekt ktory zostanie utrwalony w DataManager i bêdzie dostepny przy zapytaniach, nie morze byc niezainicjowany - isNull musi byæ false!!
    virtual void addData(const core::ObjectWrapperPtr & data, const core::DataInitializerPtr & initializer = core::DataInitializerPtr());

    virtual const core::ObjectWrapperPtr & getObjectWrapperForRawPtr(const void * ptr) const;

    virtual bool objectIsManaged(const void * ptr) const;

    virtual core::ObjectWrapperPtr createObjectWrapper(const core::TypeInfo & type) const;

    // core::IFileDataManager
public:

    //! \param files Zbiór plików ktrymi aktualnie zarz¹dza ten DataManager
    virtual void getManagedFiles(core::Files & files) const;

    virtual bool isFileManaged(core::Filesystem::Path & file) const;

    //! \param files Lista plików dla których zostan¹ utworzone parsery i z których wyci¹gniête dane
    //! bêda dostepne poprzez DataMangera LENIWA INICJALIZACJA
    virtual void addFile(const core::Filesystem::Path & file, std::vector<core::ObjectWrapperPtr> & objects = std::vector<core::ObjectWrapperPtr>());

    //! \param files Lista plików które zostan¹ usuniête z aplikacji a wraz z nimi skojarzone parsery i dane
    virtual void removeFile(const core::Filesystem::Path & file);

    //! \param path Œciezka pliku który chemy za³adowaæ (parsowaæ) WYMUSZAMY PARSOWANIE I INICJALIZACJE
    virtual void initializeFile(const core::Filesystem::Path & file);

    //! \param path Œciezka pliku który chemy za³adowaæ (parsowaæ) ZWALNIAMY ZASOBY, ALE ZEZWALAMY PONOWNIE NA LENIWA INICJALIZACJE
    virtual void deinitializeFile(const core::Filesystem::Path & file);

    //! \param files Zbior plikow dla ktorych chcemy pobrac liste obiektow
    //! \return Mapa obiektow wzgledem plikow z ktorych pochodza
    virtual void  getObjectsForFile(const core::Filesystem::Path & file, std::vector<core::ObjectWrapperPtr> & objects) const;

    //! \return true jeœli rozszerznie jest wspierane przez DataManager, w przeciwnym wypadku false
    virtual bool isExtensionSupported(const std::string & extension) const;

    //! \return Zbior obslugiwanych rozszerzen plikow wraz z ich opisem
    virtual const Extensions & getSupportedFilesExtensions() const;

    virtual const ExtensionDescription & getExtensionDescription(const std::string & extension) const;
};

#endif // DATA_MANAGER_H
