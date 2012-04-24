/**
@author Marek Daniluk
@brief Klasa DataManager implementuje interfejs core::IDataManager. S�u�y do zarz�dzania zasobami edytora, kt�re dziel� si� na dane
konfiguracyjne i pr�by pomiarowe. Zasoby �adowane s� z odpowiednich �cie�ek, domy�lnie zasoby sta�e powinny znajdowa� si� w
resources, pr�by pomiarowe s� wyszukiwane i pobierane do trials.
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
    //! Deklaracja wewn�trznej reprezentacji parsera, obudowauj�cej core::IParser
    class Parser;

    //! Wska�nik na parser.
    typedef core::shared_ptr<Parser> ParserPtr;
    //! S�aby wska�nik na parser
    typedef core::weak_ptr<Parser> ParserWeakPtr;
    // TODO: zastanwoi� si� nad u�yciem s�abych wska�nik�w


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
        //! \return Czy obiekt wspiera okre�lony typ?
        virtual bool isSupported(const core::TypeInfo& type) const;
        //! \param type Typ inteligentnego wska�nika.
        //! \return true je�eli do z obiektu mo�na wy�uska� dany wska�nik.
        virtual bool isPtrSupported(const core::TypeInfo& type) const;

        //! \return Informacje o typie.
        virtual core::TypeInfo getTypeInfo() const;

        //! \return Informacje o typie odpowiednio normalnego i sta�ego wska�nika.
        virtual std::pair<core::TypeInfo, core::TypeInfo> getPtrTypeInfo() const;

        //! \param supported Lista wspieranych rozszerze�.
        virtual void getSupportedTypes(Types& supported) const;

        //! \return Czy wrappowany obiekt jest wyzerowany?
        virtual bool isNull() const;

        //! \return Klon bie��cego obiektu. Wewn�trzny wska�nik r�wnie� jest kopiowany.
        virtual core::ObjectWrapperPtr clone() const;

        virtual core::ObjectWrapperPtr create() const;

    protected:

        virtual bool setSmartPtr(const void* ptr, const core::TypeInfo& type);

        //! \return Czy uda�o si� ustawi� m�dry wska�nik?
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

    //! ----------------------------- Dane sta�e w Datamanager ---------------------------------

    //! Dane sta�e na temat rozszerzenia
    struct ExtendedExtensionDescription : public ExtensionDescription
    {
        //! Prototypu parser�w wspierajace to rozszerzenie
        std::set<core::IParserPtr> parsers;
    };

    //! Kolekcja parser�w zarzadzanych przez DataManager
    typedef std::set<ParserPtr> Parsers;

    //! S�ownik prototyp�w parser�w wed�ug ID.
    typedef std::map<UniqueID, core::IParserPtr> IParsersByID;

    //! S�ownik wspieranych przez aplikacje rozszerze� plik�w poprzez mechanizm parser�w
    typedef std::map<std::string, ExtendedExtensionDescription> SupportedExtensionsPersistenceData;

    //! ---------------------------------------------------------------------------------

    //! -------------------- Struktury do obs�ugi danych pod postacia ObjectWrapper�w i Parser�w ------------------

    typedef std::map<core::ObjectWrapperPtr, core::DataInitializerPtr> ObjectsWithInitializers;

    //! S�ownik prawdziwych parser�w i zwi�zanych z nimi obiekt�w, kt�re mo�na pobra� z DataManagera
    typedef std::map<ParserPtr, core::Objects> ObjectsByParsers;
    //! S�ownik obiekt�w domenowych za�adowanych do DataManger i odpowiadaj�cych im parser�w
    typedef std::map<core::ObjectWrapperPtr, ParserPtr> ParsersByObjects;
    //! S�ownik aktualnie obs�ugiwanych plik�w i skojarzonych z nimi parser�w
    typedef std::map<core::Filesystem::Path, Parsers> ParsersByFiles;
    //! Typ mapy obiekt�w.
    typedef std::map< core::TypeInfo, core::Objects > ObjectsByTypes;
    //! Mapa surowych wska�nik�w i odpowiadaj�cych im ObjectWrapper�w
    typedef std::map<const void *, core::ObjectWrapperPtr> RawObjectWrapperMapping;

	//! -------------------- Struktury do obs�ugi metadany ObjectWrapper�w ------------------

	//! Metadane wg OW
	typedef std::map<core::ObjectWrapperPtr, std::vector<core::ObjectWrapperPtr>> MetadataByObjects;

    //! ---------------------------------------------------------------------------------

    //! Mapa fabryk obiekt�w.
    typedef std::map<core::TypeInfo, core::IObjectWrapperFactoryPtr> ObjectFactories;
    //! Mapa typ�w i prototyp�w ich ObjectWrapper�w
    typedef std::map<core::TypeInfo, core::ObjectWrapperConstPtr> RegisteredTypesPrototypes;
    //! S�ownik hierarchii typow -> mapuje typ do jego typow bazowych (hierarchia dziedziczenia) [first] i do typow po nim dziedziczacych [second]
    typedef std::map<core::TypeInfo, std::pair<core::TypeInfoSet, core::TypeInfoSet> > TypesHierarchy;

private:
    //! S�owniki parser�w niezainicjalizowanych.
    IParsersByID registeredParsers;    

    //! S�ownik parser�w przypisanych do rozszerze�.
    SupportedExtensionsPersistenceData registeredExtensions;

    //! Zarejestrowane rozszerzenia plik�w
    Extensions extensions;

    //! Zbi�r zarz�dzanych przez DM ObjectWrapper�w
    core::Objects objects;

    //! Mapa obiekt�w i ich inicjalizator�w
    ObjectsWithInitializers objectsWithInitializers;

    //! Mapa obiekt�w wg parser�w
    ObjectsByParsers objectsByParsers;

    //! Mapa parser�w wg obiekt�w
    ParsersByObjects parsersByObjects;

    //! Mapa parser�w wg plik�w
    ParsersByFiles parsersByFiles;

    //! Obiekty pochodz�ce z parser�w.
    ObjectsByTypes objectsByTypes;

	//! Metadane wg OW
	MetadataByObjects metadataByObjects;

    //! Mapowanie surowcyh wska�nik�w do ObjectWrapper�w - u�ywane przy obs�udze wymiany danych w Workflow
    RawObjectWrapperMapping rawPointerToObjectWrapper;

    //! S�ownik fabryk typ�w.
    ObjectFactories objectFactories;

    //! Prototypy ObjecWrapper�w zarejestrowanych typ�w danych
    RegisteredTypesPrototypes registeredTypesPrototypes;

    //! Zarejestrowane w aplikacji typy danych
    core::Types registeredTypes;

    //! Hierarchia zaresjetrowanych typ�w danych
    TypesHierarchy typesHierarchy;

    //! Synchronizacja operacji na DM
    mutable OpenThreads::ReentrantMutex stateMutex;

public:
    //! \param extension Rozszerzenie, kt�re doprowadzamy do formy obs�ugiwanej przez DM - zaczynamy od kropki, wszystko ma�ymi literami
    static void prepareExtension(std::string & extension);

    //! \param factory Fabryka ObjectWrapper�w zadanego typu dostarczana wraz z nowym typem rejestorwanym w aplikacji
    void registerObjectFactory(const core::IObjectWrapperFactoryPtr & factory);

    //! \param type Typ dla kt�rego chcemy utworzy� ObjectWrapperCollection
    //! \return ObjectWrapperCollection dla zadanego typu
    virtual core::ObjectWrapperCollectionPtr createWrapperCollection(const core::TypeInfo& typeInfo);

    //! \param type Typ dla kt�rego chcemy dosta� prototyp ObjectWrappera
    //! \return prototyp ObjectWrapper dla zadanego typu
    const core::ObjectWrapperConstPtr & getTypePrototype(const core::TypeInfo & typeInfo) const;

    ////! \param sourceTypeInfo Typ z ktorego chcemy pobrac dane
    ////! \param destTypeInfo Typ do ktorego chcemy zapisac dane
    ////! \return true jesli typ �r�d�owy wspiera typ docelowy lub sa identyczne
    bool isTypeCompatible(const core::TypeInfo & sourceTypeInfo, const core::TypeInfo & destTypeInfo) const;

    //! Rejestruje zadany parser.
    //! \param newService
    void registerParser(const core::IParserPtr & parser);
    //! \return Liczba niezainicjalizowanych parser�w.
    int getNumRegisteredParsers() const;
    //! \param idx Indeks parsera.
    //! \return Parser o zadanym indeksie. Parser zawsze b�dzie niezainicjowany.
    core::IParserConstPtr getRegisteredParser(int idx) const;

//IDataManagerReader
public:

    virtual void getObjects(std::vector<core::ObjectWrapperConstPtr>& objects, const core::TypeInfo& type, bool exact = true);

    virtual void getObjects(core::ObjectWrapperCollection& objects);

	virtual bool isManaged(const core::ObjectWrapperConstPtr & object) const;

	//! \param object Obiekt dla kt�rego pobieram metadane
	//! \param metadataCollection [out] Kolekcja metadanych dla zadanego obiektu
	virtual void getMetadataForObject(const core::ObjectWrapperConstPtr & object, std::vector<core::ObjectWrapperConstPtr> & metadataCollection) const;
	//! \param typeInfo Typ danych dla kt�rych chcemy pobrac wszystkie metadane
	//! \param metadataCollection [out] Kolekcja metadanych dla zadanego obiektu
	//! \param exact Czy mamy do�ancza� metadane typ�w pochodnych
	virtual void getMetadataForType(const core::TypeInfo & typeInfo, std::vector<core::ObjectWrapperConstPtr> & metadataCollection, bool exact = true) const;

    //! \return Zarejestrowane w aplikacji typy danych
    virtual const core::TypeInfoSet & getSupportedTypes() const;

    //! \return Hierarchia typow danych - jakie operacje moge realizowac, po czym dziedzicze
    virtual const core::TypeInfoSet & getTypeBaseTypes(const core::TypeInfo & type) const;

    //! \return Hierarchia typow danych - jakie typy po mnie dziedzicza, kto wspiera moj interfejs i moze byc downcastowany na mnie
    virtual const core::TypeInfoSet & getTypeDerrivedTypes(const core::TypeInfo & type) const;

    //core::IMemoryDataManager
public:

    //! \param objects Zbi�r obiekt�w domenowych zarz�dzanych przez ten DataManager
    virtual void getManagedData(core::Objects & objects) const;

    //! \param Obiekt ktory chcemy inicjalizowa�
    virtual void initializeData(core::ObjectWrapperPtr & data);

    //! \param Obiekt dla ktorego chcemy wykonac deinicjalizacje
    virtual bool isInitializable(const core::ObjectWrapperPtr & data) const;

    //! \param Obiekt ktory chcemy deinicjalizowa� - dalej jest w DataManager ale nie zawiera danych - trzeba potem inicjalizowa�
    virtual void deinitializeData(core::ObjectWrapperPtr & data);

    //! \param Obiekt ktory zostanie usuniety jesli zarzadza nim DataManager
    virtual void removeData(const core::ObjectWrapperPtr & data);

private:

    //! \param Obiekt ktory zostanie utrwalony w DataManager i b�dzie dostepny przy zapytaniach, nie morze byc niezainicjowany - isNull musi by� false!!
    virtual void addData(const core::ObjectWrapperPtr & data, const core::DataInitializerPtr & initializer = core::DataInitializerPtr());

    virtual const core::ObjectWrapperPtr & getObjectWrapperForRawPtr(const void * ptr) const;

    virtual bool objectIsManaged(const void * ptr) const;

    virtual core::ObjectWrapperPtr createObjectWrapper(const core::TypeInfo & type) const;

    // core::IFileDataManager
public:

    //! \param files Zbi�r plik�w ktrymi aktualnie zarz�dza ten DataManager
    virtual void getManagedFiles(core::Files & files) const;

    virtual bool isFileManaged(core::Filesystem::Path & file) const;

    //! \param files Lista plik�w dla kt�rych zostan� utworzone parsery i z kt�rych wyci�gni�te dane
    //! b�da dostepne poprzez DataMangera LENIWA INICJALIZACJA
    virtual void addFile(const core::Filesystem::Path & file, std::vector<core::ObjectWrapperPtr> & objects = std::vector<core::ObjectWrapperPtr>());

    //! \param files Lista plik�w kt�re zostan� usuni�te z aplikacji a wraz z nimi skojarzone parsery i dane
    virtual void removeFile(const core::Filesystem::Path & file);

    //! \param path �ciezka pliku kt�ry chemy za�adowa� (parsowa�) WYMUSZAMY PARSOWANIE I INICJALIZACJE
    virtual void initializeFile(const core::Filesystem::Path & file);

    //! \param path �ciezka pliku kt�ry chemy za�adowa� (parsowa�) ZWALNIAMY ZASOBY, ALE ZEZWALAMY PONOWNIE NA LENIWA INICJALIZACJE
    virtual void deinitializeFile(const core::Filesystem::Path & file);

    //! \param files Zbior plikow dla ktorych chcemy pobrac liste obiektow
    //! \return Mapa obiektow wzgledem plikow z ktorych pochodza
    virtual void  getObjectsForFile(const core::Filesystem::Path & file, std::vector<core::ObjectWrapperPtr> & objects) const;

    //! \return true je�li rozszerznie jest wspierane przez DataManager, w przeciwnym wypadku false
    virtual bool isExtensionSupported(const std::string & extension) const;

    //! \return Zbior obslugiwanych rozszerzen plikow wraz z ich opisem
    virtual const Extensions & getSupportedFilesExtensions() const;

    virtual const ExtensionDescription & getExtensionDescription(const std::string & extension) const;
};

#endif // DATA_MANAGER_H
