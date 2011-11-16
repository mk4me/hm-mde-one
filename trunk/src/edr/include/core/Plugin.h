/********************************************************************
	created:  2010/10/04
	created:  4:10:2010   15:30
	filename: Plugin.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD__CORE_PLUGIN_H__
#define __HEADER_GUARD__CORE_PLUGIN_H__

#include <vector>
#include <string>

#include <boost/version.hpp>
#include <QtCore/QtGlobal>
#include <utils/Macros.h>

#include <core/PluginCommon.h>
#include <core/SmartPtr.h>
#include <core/IService.h>
#include <core/IParser.h>
#include <core/IVisualizer.h>
#include <core/IDataProcessor.h>
#include <core/Export.h>
#include <core/ObjectWrapperFactory.h>

////////////////////////////////////////////////////////////////////////////////
namespace core {
////////////////////////////////////////////////////////////////////////////////

//! Deklaracja wersji nag≥Ûwka w Pluginie
//! NIEZWYKLE ISTOTNE - przy kaødej zmianie w Pluginach wersja ta bÍdzie siÍ zmieniaÊ, pluginy nieprzebudowane z nowym nag≥owiekm
//! nie bÍdπ ≥adowane do aplikacji!!

//! Sami musimy modyfikowaÊ ta wersjÍ!!
#define CORE_PLUGIN_INTERFACE_VERSION 3

#ifdef _CPPLIB_VER
#define CORE_CPPLIB_VER _CPPLIB_VER
#else
#define CORE_CPPLIB_VER -1
#endif

//! Nazwa funkcji pobierajπcej numer wersji pluginu.
#define CORE_GET_PLUGIN_VERSION_FUNCTION_NAME CoreGetPluginInterfaceVersion
//! Nazwa funkcji tworzπcej plugin.
#define CORE_CREATE_PLUGIN_FUNCTION_NAME CoreCreatePluginInstance
//! 
#define CORE_GET_LIBRARIES_VERSIONS_FUNCTION_NAME CoreGetLibrariesVersions

//! Rozpoczyna rejestracjÍ pluginu.
//! \param name Nazwa pluginu.
//! \param id ID pluginu.
#define CORE_PLUGIN_BEGIN(name, id)                                     \
CORE_DEFINE_INSTANCE_INFO                                               \
DEFINE_DEFAULT_LOGGER("edr." name)                                      \
extern "C" CORE_EXPORT unsigned CORE_GET_PLUGIN_VERSION_FUNCTION_NAME() \
{                                                                       \
    return CORE_PLUGIN_INTERFACE_VERSION;                               \
}                                                                       \
extern "C" CORE_EXPORT void CORE_GET_LIBRARIES_VERSIONS_FUNCTION_NAME(  \
    int* boostVersion, int* qtVersion, int* stlVersion)                 \
{                                                                       \
    *boostVersion = BOOST_VERSION;                                      \
    *qtVersion = QT_VERSION;                                            \
    *stlVersion = CORE_CPPLIB_VER;                                      \
}                                                                       \
extern "C" CORE_EXPORT core::Plugin* CORE_CREATE_PLUGIN_FUNCTION_NAME(core::InstanceInfo* data) \
{                                                                       \
    core::__instanceInfo = *data;                                       \
    core::Plugin* instance = new core::Plugin((name), (id));            

//! KoÒczy rejestracjÍ pluginu.
#define CORE_PLUGIN_END                                                 \
    return instance;                                                    \
}

//! Dodaje us≥ugÍ zadanego typu do pluginu.
#define CORE_PLUGIN_ADD_SERVICE(className)                              \
    instance->addService( core::IServicePtr(new className) );

//! Dodaje parser zadanego typu do pluginu.
#define CORE_PLUGIN_ADD_PARSER(className)                               \
    instance->addParser( core::IParserPtr(new className) );

//! Dodaje wizualizator zadanego typu do pluginu.
#define CORE_PLUGIN_ADD_VISUALIZER(className)                           \
    instance->addVisualizer( core::IVisualizerPtr(new className) );

//! Dodaje elementu przetwarzajπcego zadanego typu do pluginu.
#define CORE_PLUGIN_ADD_DATA_PROCESSOR(className)                           \
    instance->addDataProcessor( core::IDataProcessorPtr(new className) );

//! Dodaje ürÛd≥o zadanego typu do pluginu.
#define CORE_PLUGIN_ADD_DATA_SOURCE(className)                           \
    instance->addDataSource( core::IDataSourcePtr(new className) );

//! Dodanie nowego typu domenowego poprzez utworzenie dla niego ObjectWrapperFactory
#define CORE_PLUGIN_ADD_OBJECT_WRAPPER(className)               \
    instance->addObjectWrapperFactory<className>();//( core::IObjectWrapperFactoryPtr(new core::ObjectWrapperFactory<className>()) ); 
    //instance->addObjectWrapperFactory( core::IObjectWrapperFactoryPtr(new core::ObjectWrapperFactory<className>()) ); 


//! Interfejs pluginu przez ktÛry dostarczane sa us≥ugi (serwisy) i prototypy elementÛw przetwarzajπcych dane
class IPlugin : IIdentifiable
{
public:
    //! Pusty polimorficzny destruktor.
    virtual ~IPlugin() {}
    //! \return Liczba us≥ug.
    virtual int getNumServices() const = 0;
    //! \return Us≥uga.
    virtual IService* getService(int idx) = 0;
    //! \return Liczba parserÛw.
    virtual int getNumParsers() const = 0;
    //! \return Parser.
    virtual IParser* getParser(int idx) = 0;
    //! \return Liczba wizualizatorÛw.
    virtual int getNumVisualizers() const = 0;
    //! \return Wizualizator.
    virtual IVisualizer* getVisualizer(int idx) = 0;
    //! \return Liczba elementÛw przetwarzajπcych.
    virtual int getNumDataProcesors() const = 0;
    //! \return Element przetwarzajπcy.
    virtual IDataProcessor* getDataProcessor(int idx) = 0;
    //! \return Liczba ürÛde≥ danych.
    virtual int getNumDataSources() const = 0;
    //! \return èrÛd≥o danych.
    virtual IDataSource* getDataSource(int idx) = 0;

};

/**
 *	Kontener na us≥ugi.
 */
class Plugin : IIdentifiable
{
public:
    //! Typ funkcji tworzπcej plugin.
    typedef Plugin* (*CreateFunction)(InstanceInfo* data);
    //! Typ funkcji pobierajacej wersjÍ pluginu.
    typedef int (*GetVersionFunction)();
    //!
    typedef int (*GetLibrariesVersionFunction)(int* boostVersion, int* qtVersion, int* stlVersion);
    //! Typ listy us≥ug.
    typedef std::vector<IServicePtr> Services;
    //! Typ listy parserÛw.
    typedef std::vector<IParserPtr> Parsers;
    //! Typ listy wizualizatorÛw.
    typedef std::vector<IVisualizerPtr> Visualizers;
    //! Typ listy elementÛw przetwarzajπcych.
    typedef std::vector<IDataProcessorPtr> DataProcessors;
    //! Typ listy ürÛde≥ danych.
    typedef std::vector<IDataSourcePtr> DataSources;
    //! Typ listy wrapperÛw.
    typedef std::vector<IObjectWrapperFactoryPtr> ObjectWrapperFactories;

private:
    //! Lista us≥ug pluginu.
    Services services;
    //! Lista parserÛw pluginu.
	Parsers parsers;
    //! Lista wizualizatorÛw pluginu.
    Visualizers visualizers;
    //! Lista elementÛw przetwarzajπcych pluginu.
    DataProcessors dataProcessors;
    //! Lista ürÛde≥ danych pluginu.
    DataSources dataSources;
    //! Lista fabryk wrapperÛw.
    ObjectWrapperFactories factories;

    //! Nazwa pluginu.
    std::string name;
    //! ID pluginu.
    UniqueID id;
    //! åcieøka do pluginu.
    std::string path;

public:
    //! \param name Nazwa pluginu.
    Plugin(const std::string& name, UniqueID id) 
        : name(name), path("UNKNOWN"), id(id)
    {
    }

public:
    //! \return ID pluginu.
    virtual UniqueID getID() const
    {
        return id;
    }     

    //! \return nazwa pluginu
    virtual const std::string & getDescription() const
    {
        return name;
    }

    //! \return Nazwa pluginu.
    const std::string& getName() const
    {
        return name;
    }
    //! \return
    const std::string& getPath() const
    { 
        return path;
    }
    //! \param path
    void setPath(const std::string& path) 
    { 
        this->path = path; 
    }

    //! \service Us≥uga do dodania do pluginu.
    void addService(const IServicePtr & service)
    {
        services.push_back(service);
    }
    //! \return Liczba us≥ug dostarczanych przez plugin.
    int getNumServices() const
    {
        return static_cast<int>(services.size());
    }
    //! \param i
    const IServicePtr & getService(int i)
    {
        return services[i];
    }
	
    //! \parser Parser do dodania do pluginu.
	void addParser(const IParserPtr & parser)
    {
        this->parsers.push_back(parser);
    }
    //! \return Liczba parserÛw dostarczanych przez plugin.
    int getNumParsers() const
    {
		return static_cast<int>(this->parsers.size());
    }
    //! \param i
	const IParserPtr & getParser(int i)
    {
        return this->parsers[i];
    }

    //! \visualizer Visualizer do dodania do pluginu.
    void addVisualizer(const IVisualizerPtr & visualizer)
    {
        this->visualizers.push_back(visualizer);
    }
    //! \return Liczba visualizerÛw dostarczanych przez plugin.
    int getNumVisualizers() const
    {
        return static_cast<int>(this->visualizers.size());
    }
    //! \param i
    const IVisualizerPtr & getVisualizer(int i)
    {
        return this->visualizers[i];
    }
    //! \param dataProcessor IDataProcessor do dodania do pluginu.
    void addDataProcessor(const IDataProcessorPtr & dataProcessor)
    {
        this->dataProcessors.push_back(dataProcessor);
    }
    //! \return Liczba elementÛw przetwarzajπcych dostarczanych przez plugin.
    int getNumDataProcessors() const
    {
        return static_cast<int>(this->dataProcessors.size());
    }
    //! \param i
    const IDataProcessorPtr & getDataProcessor(int i)
    {
        return this->dataProcessors[i];
    }

    //! \param dataSource èrÛd≥o danych do dodania do pluginu.
    void addDataSource(const IDataSourcePtr & dataSource)
    {
        this->dataSources.push_back(dataSource);
    }
    //! \return Liczba ürÛde≥ danych dostarczanych przez plugin.
    int getNumDataSources() const
    {
        return static_cast<int>(this->dataSources.size());
    }
    //! \param i
    const IDataSourcePtr & getDataSource(int i)
    {
        return this->dataSources[i];
    }

    //! \param factory
    template<class T>
    void addObjectWrapperFactory(const T * dummy = nullptr/*const IObjectWrapperFactoryPtr & factory*/)
    {
        //this->factories.push_back(factory);
        this->factories.push_back(IObjectWrapperFactoryPtr(new ObjectWrapperFactory<T>()));
    }
    //! \return
    int getNumWrapperFactories() const
    {
        return static_cast<int>(this->factories.size());
    }
    //! \param i
    const IObjectWrapperFactoryPtr & getWrapperFactory(int i)
    {
        return this->factories[i];
    }
};

//! Definicja wskaünika.
typedef shared_ptr<Plugin> PluginPtr;
typedef shared_ptr<const Plugin> PluginConstPtr;

////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////


#endif  // __HEADER_GUARD__CORE_PLUGIN_H__